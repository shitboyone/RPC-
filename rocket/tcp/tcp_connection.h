#ifndef ROCKET_TCP_TCP_CONNECTION_H
#define ROCKET_TCP_TCP_CONNECTION_H
#include "rocket/tcp/net_addr.h"
#include "rocket/tcp/tcp_buffer.h"
#include "rocket/net/io_thread.h"
#include "rocket/net/coder/abstract_protcol.h"
#include "rocket/net/coder/abstract_coder.h"
#include "rocket/net/coder/tinypb_protocol.h"
#include "rocket/net/rpc/rpc_dispatcher.h"
#include <memory>
#include <queue>
namespace rocket{

    enum TcpState{
                NotConnected=-1,
                Connect=0,
                HalfClosing=1,
                Closed=2,
        };
    enum Tcpconnectiontype{
        TcpConnectionByserver=1,
        TcpConnectionByclient=2,
    };    

    class TcpConnection{
        public:
            
            typedef std::shared_ptr<TcpConnection> s_ptr;

            TcpConnection(EventLoop *eventloop,int fd,
                int buffer_size,NetAddr::s_ptr peer_addr,NetAddr::s_ptr local_add,Tcpconnectiontype type=TcpConnectionByserver);

            ~TcpConnection();

            void onread();

            void excute();

            void onwrite();

            void setState(const TcpState state);

            TcpState getState();

            void clear();

            FdEvent* getFdEvent(){
                return m_fd_event;
            }

            //服务器主动关闭
            void shutdown();

            void SetConnectionType(Tcpconnectiontype type);

            void listenWrite();
            void listenRead();

            void pushSentMessage(AbstractProtocol::s_ptr message,std::function<void(AbstractProtocol::s_ptr)>done);
            void pushReadMessage(const std::string &req_id,std::function<void(AbstractProtocol::s_ptr)>done);

            NetAddr::s_ptr getPeerAddr();
            NetAddr::s_ptr getLocalAddr();

        private:
            NetAddr::s_ptr m_local_addr;
            NetAddr::s_ptr m_peer_addr;    

            TCpBUffer::s_ptr m_in_buffer;  //接受缓冲区
            TCpBUffer::s_ptr m_out_buffer;   //发送缓冲区

            EventLoop *m_event_loop {nullptr};  //持有当前连接的线程

            FdEvent *m_fd_event {nullptr};

            TcpState m_state;

            int m_fd{-1};

            AbstractCoder *m_coder;

            Tcpconnectiontype m_connection_type {TcpConnectionByserver};

            //
            std::vector<std::pair<AbstractProtocol::s_ptr,std::function<void(AbstractProtocol::s_ptr)>>>m_write_dones;

            //key为req_id
            std::map<std::string,std::function<void(AbstractProtocol::s_ptr)>>m_read_dones;

    };

}


#endif