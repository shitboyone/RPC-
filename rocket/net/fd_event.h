#ifndef ROCKET_NET_FD_EVENT_H
#define ROCKET_NET_FD_EVENT_H
#include <functional>
#include <sys/epoll.h>
// #include <functional>
namespace rocket{

    class FdEvent{
        
        public:

            enum TriggerEvent{
                IN_EVENT=EPOLLIN,
                OUT_EVENT=EPOLLOUT,
            };

            FdEvent(int fd);

            FdEvent();

            virtual ~FdEvent(){}

            std::function<void ()> handle(TriggerEvent event_type);

            void listen(TriggerEvent event_type,std::function<void ()>callback);

            void setNonBlock();

            int getFD()const{
                return m_fd;
            }

            void cancel(TriggerEvent event_type);

            epoll_event getEpollEvent(){
                return m_listen_events;
            }
 
        protected:

            epoll_event m_listen_events;
            int m_fd{-1};
            std::function<void ()>m_read_callback;
            std::function<void ()>m_write_callback;
    };

}


#endif