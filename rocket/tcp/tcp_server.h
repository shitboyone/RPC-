#ifndef ROCKET_TCP_TCP_SERVER_H
#define  ROCKET_TCP_TCP_SERVER_H
#include "rocket/tcp/tcp_acceptor.h"
#include "rocket/tcp/net_addr.h"
#include "rocket/net/EventLoop.h"
#include "rocket/net/io_thread_group.h"
#include "rocket/tcp/tcp_connection.h"
namespace rocket
{
    class TcpServer{                  //全局的单例对象，只能在主线程中调用
        public:
          TcpServer(NetAddr::s_ptr addr);
          ~TcpServer();
          void start();
          void init();
        private:
          void onAccept();

        
        private:
        int client_count{0};
        TcpAcceptor::s_ptr m_acceptor{nullptr};
        NetAddr::s_ptr m_local_addr{nullptr};
        EventLoop *m_main_event_loop{nullptr};  //主事件循环
        IOThreadGroup *m_io_thread_loop{nullptr};   //subreactor 组
        FdEvent *m_lsitend_fd_event{nullptr};

        std::set<TcpConnection::s_ptr>m_client;

    };
} // namespace socket




#endif