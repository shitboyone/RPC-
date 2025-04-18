#ifndef ROCKET_NET_EVENTLOOP_H

#define ROCKET_NET_EVENTLOOP_H

#include <pthread.h>
#include <set>
#include <functional>
#include <queue>
#include <mutex>
#include "rocket/net/fd_event.h"
#include "rocket/net/wakeup_fd_event.h"
#include "rocket/net/timer.h"
namespace rocket{

    class EventLoop{
        public:
            EventLoop();

            void loop();

            void wakeup();

            void stop();

            void addEpollEvent(FdEvent *event );

            void deleteEpollEvent(FdEvent *event );

            bool isTnLoopThread();

            void addTask(std::function<void ()>cb,bool iswakeup=false);

            void addTimerEvent(TimerEvent::s_ptr event);

            void initWakeUp();

            void iniTimer();

            static EventLoop * getCurrentEventLoop();

            ~EventLoop();

            bool Islooping();

        private:
            pid_t m_pid{0};

            int m_epoll_fd{0};

            int m_wakeup_fd {0};

            WakeUpEevent *m_wake_fd_event{NULL};

            bool m_stop_flag{false};
            
            std::set<int>m_listend_fds;

            std::queue<std::function<void()>>m_pending_tasks;

            std::mutex m_mu;

            Timer *m_timer{nullptr};

            

        private:

        void dealwakeup();

        bool is_looping{false};

        //std::set<int>m_listed_fd;


    };


       

}



#endif