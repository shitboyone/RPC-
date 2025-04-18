#include "rocket/tcp/tcp_client.h"
#include "rocket/common/log.h"
#include "rocket/net/EventLoop.h"
#include "rocket/tcp/fd_event_group.h"
#include "rocket/tcp/tcp_connection.h"
#include <unistd.h>
namespace rocket{
    TcpcClient::TcpcClient(NetAddr::s_ptr peer_addr):m_peer_addr(peer_addr){
        m_event_loop=EventLoop::getCurrentEventLoop();
        m_fd=socket(peer_addr->getFamiliy(),SOCK_STREAM,0);
        if(m_fd<0){
            ERRORLOG("TCPclient::TCPclient() errom,fialed to create fd");
            return ;
        }
        m_fd_event=FdEventGroup::GetFdEVentGroup()->getFdEvent(m_fd);
        m_fd_event->setNonBlock();
        m_connection=std::make_shared<TcpConnection>(m_event_loop,m_fd,128,peer_addr,nullptr,TcpConnectionByclient);
        m_connection->SetConnectionType(TcpConnectionByclient);

     }

     TcpcClient::~TcpcClient(){
        if(m_fd>0){
            close(m_fd);
        }
    }


    //异步进行connect，如果connect成功，执行回调函数done
    void TcpcClient::connect(std::function<void()>done){
        
        int rt=::connect(m_fd,m_peer_addr->getSocketAddr(),m_peer_addr->getSocketlen());
        if(rt==0){
            DEBUGLOG("connect [%s] succ",m_peer_addr->to_string().c_str());
            if(done){
                done();
                m_connection->setState(Connect);
            }
        }else if(rt==-1){
            if(errno == EINPROGRESS) {
                //epoll监听可读事件，判断错误码
                m_fd_event->listen(FdEvent::OUT_EVENT,[this,done](){
                    int error=0;
                    bool is_connect=false;
                    socklen_t error_len=sizeof(error);
                    getsockopt(m_fd,SOL_SOCKET,SO_ERROR,&error,&error_len);
                    if(error == 0){
                        DEBUGLOG("connect [%s] succ",m_peer_addr->to_string().c_str());
                        m_connection->setState(Connect);
                        is_connect=true;
                    }else{
                            ERRORLOG("connect [%s] faile ",m_peer_addr->to_string().c_str());
                    }
                    //取消写事件监听
                    m_fd_event->cancel(FdEvent::OUT_EVENT);
                    m_event_loop->addEpollEvent(m_fd_event);
                    //连接成功执行回调
                    if(is_connect&&done){
                        done();
                    }
                });
                m_event_loop->addEpollEvent(m_fd_event);
                if(!(m_event_loop->Islooping())){
                    m_event_loop->loop();
                }
            }
        }
        else{
            ERRORLOG("connect [%s] faile ",m_peer_addr->to_string().c_str());
        }

    }

    //异步发送message，如果成功调用done函数
    void TcpcClient::writeMessage(AbstractProtocol::s_ptr message,std::function<void(AbstractProtocol::s_ptr)>done){
        //1将message对象写入Connection的buffer
        //2:启动connection可写事件
        m_connection->pushSentMessage(message,done);
        m_connection->listenWrite();
    }


    //异步的读取message，如果成功，调用done函数
    void TcpcClient::readMessage(const std::string&req_id,std::function<void(AbstractProtocol::s_ptr)>done){
        //1：监听可读
        //2：从buffer里decodemessage对象,判断是否req_id相等，相等成功，执行回调
        m_connection->pushReadMessage(req_id,done);
        m_connection->listenRead();
}

     void TcpcClient::stop(){
        if(m_event_loop->Islooping()){
            m_event_loop->stop();
            //唤醒epoll
            m_event_loop->wakeup();
        }
     }

     
    void TcpcClient::addTimerEvent(TimerEvent::s_ptr timer_event){
        m_event_loop->addTimerEvent(timer_event);
    }

}