#include "rocket/tcp/tcp_connection.h"
#include "rocket/common/log.h"
#include "rocket/tcp/fd_event_group.h"
#include <unistd.h>
#include "rocket/tcp/tcp_buffer.h"
#include <cstring>
#include "rocket/net/coder/abstract_protcol.h"
#include "rocket/net/coder/string_code.h"
#include "rocket/net/coder/tinypb_protocol.h"
#include "rocket/net/coder/tinypb_coder.h"
namespace rocket{
    TcpConnection::TcpConnection(EventLoop *eventloop,int fd,int buffer_size,NetAddr::s_ptr peer_addr,NetAddr::s_ptr local_addr,Tcpconnectiontype type) 
    :m_event_loop(eventloop),m_peer_addr(peer_addr),m_local_addr(local_addr),m_state(NotConnected),m_fd(fd),m_connection_type(type){
        INFOLOG("TcpConnection ");
        m_in_buffer=std::make_shared<TCpBUffer>(buffer_size);
        m_out_buffer=std::make_shared<TCpBUffer>(buffer_size);

        m_fd_event=FdEventGroup::GetFdEVentGroup()->getFdEvent(fd);
        INFOLOG("client fd_event=%d",m_fd_event->getFD());
        m_fd_event->setNonBlock();
        //事件添加到eventloop中
        if(m_connection_type==TcpConnectionByserver){
            listenRead();
        }
        m_coder=new TinyPBCoder();
        //INFOLOG("cleintfd=%d,succ register epoll read event on thread=%d",m_fd,m_io_thread->getpid());
    }

    TcpConnection::~TcpConnection(){
        DEBUGLOG("~tcp_connection");
        clear();
        if(m_coder){
            delete m_coder;
            m_coder=nullptr;
        }
    }

    void TcpConnection::onread(){
        //从socket的系统缓存区，读出到in_buffer缓冲区中
        if(m_state!=Connect){
            INFOLOG("client has already disconnected,add[%s],clientfd[%d]",m_peer_addr->to_string().c_str(),m_fd);
        }

        bool is_read_all=false;
        bool is_close=false;
        while(!is_read_all){
            if(m_in_buffer->writeAble()==0){
                m_in_buffer->resizeBuffer(2*m_in_buffer->m_buffer.size());
            }
            int read_count=m_in_buffer->writeAble();
            int write_index=m_in_buffer->writeIndex();
            INFOLOG("read_count=%d",read_count)
            char buff[128];
            memset(buff,0,sizeof(buff));
            //int rt=read(m_fd,buff,128);
            int rt=read(m_fd,&(m_in_buffer->m_buffer[write_index]),read_count);
            DEBUGLOG("succ read %d bytes from addr [%s],client fd [%d]",rt,m_peer_addr->to_string().c_str(),m_fd);
            if(rt >0){
                m_in_buffer->moveWriteIndex(rt);
                if(rt==read_count){
                    continue;
                }else if(rt<read_count){
                    INFOLOG("read all");
                    is_read_all=true;
                    break;
                }
            }else if(rt==0){
                is_close=true;
                break;
            }else if(rt==-1 && errno==EAGAIN){
                is_read_all=true;
                break;
            }
        }
        if(is_close){
            INFOLOG("peer closed,peer addr [%s],clientfd [%d]",m_peer_addr->to_string().c_str(),m_fd);
            clear();
            return ;
        }
        if(!is_read_all){
            ERRORLOG("not read all data");
        }
        //Todo 补充RPC协议解析
        excute();
        INFOLOG("onread succ");
        

}

    void TcpConnection::excute(){
        if(m_connection_type==TcpConnectionByserver){    //当前连接是服务端
            //获取rpc请求，再把rpc响应发送回去
            // std::vector<char> tmp;
            // int size = m_in_buffer->readAble();
            // INFOLOG("read bytes is %d",size);
            // tmp.resize(size);
            // m_in_buffer->readFromBuffer(tmp,size);

            // std::string msg;
            // for(int i=0;i<tmp.size();i++){
            //     // std::cout<<tmp[i]<<endl;
            //     msg+=tmp[i];
            // }
            // printf("msgsize=%d,msg=%s\n", msg.size(),msg.data());
            // // INFOLOG("success get request from client[%s],msg is %d",m_peer_addr->to_string().c_str(),msg.size());

            // m_out_buffer->writeToBUffer(msg.c_str(),msg.length());
            
            std::vector<AbstractProtocol::s_ptr>result;
            std::vector<AbstractProtocol::s_ptr>replay_message;
            m_coder->decode(result,m_in_buffer); 
            for(size_t i=0;i<result.size();i++){
                //将消息进行处理
                //将处理后的消息放入replay_message中
                INFOLOG("get message from client[%s],msg_id=%s",m_peer_addr->to_string().c_str(),result[i]->getReqId().c_str());
                std::shared_ptr<TinyPBProtocol>replay=std::make_shared<TinyPBProtocol>();
                // replay->m_pb_data="hello this is rpc rocket test";
                // replay->m_msg_id=result[i]->m_msg_id;
                
                RpcDispatcher::GetRpcDispatcher()->dispatch(result[i],replay,this);

                replay_message.push_back(replay);
            }
            m_coder->encode(replay_message,m_out_buffer);
            //将数据写入到socket的发送缓存区
            listenWrite(); 
            // DEBUGLOG("io_thread=%d,succ add fd=%d on evenloop",m_io_thread->getpid(),m_fd);
        }else{            //当前连接是客户端
             //buffer里encode得到message对象
             std::vector<AbstractProtocol::s_ptr>result;
             m_coder->decode(result,m_in_buffer);

             for(size_t i=0;i<result.size();i++){
                std::string req_id=result[i]->getReqId();
                auto it=m_read_dones.find(req_id);
                if(it!=m_read_dones.end()){
                    it->second(result[i]);
                }   
             }
        }
        
    }

    void TcpConnection::onwrite(){

        if(m_state!=Connect){
            ERRORLOG("onwrite error,client has already disconnected addr [%s],clientfd[%d]",m_peer_addr->to_string().c_str(),m_fd);
            return ;
        }
        if(m_connection_type==TcpConnectionByclient){
            //1:将数据序列化，再放入buffer
            std::vector<AbstractProtocol::s_ptr>message;

            for(size_t i=0;i<m_write_dones.size();i++){
                message.push_back(m_write_dones[i].first);
            }

            m_coder->encode(message,m_out_buffer);
        }
        bool is_write_all=false;
        while(true){        
            if(m_out_buffer->readAble()==0){
                DEBUGLOG("no data need to send client [%s]",m_peer_addr->to_string().c_str());
                is_write_all=true;
                break;
            }
            int write_size=m_out_buffer->readAble();
            int read_index=m_out_buffer->readIndex();

            int rt=write(m_fd,&(m_out_buffer->m_buffer[read_index]),write_size);
            INFOLOG("send %d bytes data to %s",rt,m_peer_addr->to_string().c_str());
            if(rt>=write_size){
                DEBUGLOG("no data need to send to client [%s]",m_peer_addr->to_string().c_str());
                is_write_all=true;
                break;
            }
            if(rt==-1&&errno==EAGAIN){
                //发送缓存区满了，不能发送了
                //等到下次fd可写再次发送
                ERRORLOG("write to faild ,error=EAGIN and rt==-1");
                break;
            }
        }
        if(is_write_all){
            m_fd_event->cancel(FdEvent::OUT_EVENT);
            m_event_loop->addEpollEvent(m_fd_event);
        }
        //执行回调函数
        if(m_connection_type==TcpConnectionByclient){
            for(size_t i=0;i<m_write_dones.size();i++){
                m_write_dones[i].second(m_write_dones[i].first);
            }
        }
        m_write_dones.clear();
    } 

    void TcpConnection::setState(const TcpState sate){
        m_state=Connect;
    }

    TcpState TcpConnection::getState(){
        return m_state;
    }   


    void TcpConnection::clear(){
        //处理一些关闭连接后的清理动作
        if(m_state == Closed){
            return ;
        }
        m_event_loop->deleteEpollEvent(m_fd_event);
        m_fd_event->cancel(FdEvent::IN_EVENT);
        m_fd_event->cancel(FdEvent::OUT_EVENT);
        m_state=Closed;
    }

    void TcpConnection::shutdown(){
        if(m_state==Closed||m_state==NotConnected){
            return ;
        }
        //处于半关闭,
        m_state=HalfClosing;

        //关闭读写你，          
        //发送了FIN报文，触发四次挥手的第一个阶段，此时连接还没有关闭
        //当fd发生可读事件，服务端收到客服端的fin报文，连接断开
        ::shutdown(m_fd,SHUT_RDWR);
    }

    void TcpConnection::SetConnectionType(Tcpconnectiontype type){
        m_connection_type=type;
    }
    //监听可写
    void TcpConnection::listenWrite(){
        m_fd_event->listen(FdEvent::OUT_EVENT,std::bind(&TcpConnection::onwrite,this));
        m_event_loop->addEpollEvent(m_fd_event);
    }

    //监听可读
    void TcpConnection::listenRead(){
        m_fd_event->listen(FdEvent::IN_EVENT,std::bind(&TcpConnection::onread,this));
        m_event_loop->addEpollEvent(m_fd_event);
    }


    void TcpConnection::pushSentMessage(AbstractProtocol::s_ptr message,std::function<void(AbstractProtocol::s_ptr)>done){
        m_write_dones.push_back(std::make_pair(message,done));
    }


    void  TcpConnection::pushReadMessage(const std::string &req_id,std::function<void(AbstractProtocol::s_ptr)>done){
        m_read_dones.insert(std::make_pair(req_id,done));
    }


    NetAddr::s_ptr TcpConnection::getPeerAddr(){
        return m_peer_addr;
    }
    NetAddr::s_ptr TcpConnection::getLocalAddr(){
        return m_local_addr;
    }

}