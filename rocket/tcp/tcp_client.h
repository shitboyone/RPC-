#ifndef ROKCET_TCP_TCP_CLIENT_H
#define ROKCET_TCP_TCP_CLIENT_H
#include "rocket/tcp/net_addr.h"
#include "rocket/net/EventLoop.h"
#include "rocket/tcp/tcp_connection.h"
#include "rocket/net/coder/abstract_protcol.h"
#include "rocket/net/timer_event.h"
#include <memory>
namespace rocket
{
    class TcpcClient{
        public:

            typedef std::shared_ptr<TcpcClient>s_ptr;
        
            TcpcClient(NetAddr::s_ptr peerr_addr);

            ~TcpcClient();


            //异步进行connect，如果connect成功，执行回调函数done
            void connect(std::function<void()>done);

            //异步发送message，如果成功调用done函数
            void writeMessage(AbstractProtocol::s_ptr message,std::function<void(AbstractProtocol::s_ptr)>done);


            //异步的读取message，如果成功，调用done函数
            void readMessage(const std::string&req_id,std::function<void(AbstractProtocol::s_ptr)>done);

            void stop();

            void addTimerEvent(TimerEvent::s_ptr timer_event);
            
            void cancleTimerEvent(TimerEvent::s_ptr timer_event);


        private:
            NetAddr::s_ptr m_peer_addr;
            EventLoop *m_event_loop{nullptr};

            int m_fd{-1};
            FdEvent*m_fd_event{nullptr};

            TcpConnection::s_ptr m_connection;

    };
} // namespace rocket

#endif

