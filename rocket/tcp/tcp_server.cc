#include "rocket/tcp/tcp_server.h"
#include "rocket/tcp/net_addr.h"
#include "rocket/tcp/tcp_acceptor.h"
#include "rocket/common/log.h"
#include "rocket/tcp/tcp_connection.h"
#include <memory>
namespace rocket{

    TcpServer::TcpServer(NetAddr::s_ptr addr):m_local_addr(addr){
        init();
       INFOLOG("server listen succ on %s",m_local_addr->to_string().c_str());
    }

    void TcpServer::init(){

        m_acceptor=std::make_shared<TcpAcceptor>(m_local_addr);

        m_main_event_loop=EventLoop::getCurrentEventLoop();
        m_io_thread_loop=new IOThreadGroup(2);
        m_lsitend_fd_event=new FdEvent(m_acceptor->getListenFd());
        m_lsitend_fd_event->listen(FdEvent::IN_EVENT,std::bind(&TcpServer::onAccept,this));
        DEBUGLOG("listend fd succ create,fd=%d",m_lsitend_fd_event->getFD());
        m_main_event_loop->addEpollEvent(m_lsitend_fd_event);

    } 

     void TcpServer::onAccept(){
            auto re=m_acceptor->accept();
            int client_fd=re.first;
            NetAddr::s_ptr peer_addr=re.second;
            client_count++;
            //将clientfd添加到任意io线程
            IOThread *io_thread=m_io_thread_loop->getIOThread();
            TcpConnection::s_ptr connection=std::make_shared<TcpConnection>(io_thread->getEventLoop(),client_fd,128,peer_addr,m_local_addr,TcpConnectionByserver);
            connection->setState(Connect);

            //将客户端connectiom持久化保存
            m_client.insert(connection);

            INFOLOG("Tcp server succ accept client=%d",client_fd);
     }

    TcpServer::~TcpServer(){
        if(m_main_event_loop){
            delete m_main_event_loop;
            m_main_event_loop=nullptr;
        }
        if(m_io_thread_loop){
            delete m_io_thread_loop;
            m_io_thread_loop=nullptr;
        }

    } 


    void TcpServer::start(){       //顺序会影响io线程的启动 
        m_io_thread_loop->start();
        m_main_event_loop->loop();
    }



}