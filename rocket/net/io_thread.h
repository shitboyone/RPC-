#ifndef ROCKET_NET_IO_THREAD_H
#define ROCKET_NET_IO_THREAD_H

#include <pthread.h>
#include <rocket/net/EventLoop.h>
#include <semaphore.h>

namespace  rocket
{
    class IOThread{
    public:
        IOThread();
        ~IOThread();

        EventLoop * getEventLoop();

        void join();

    public:
        static void * Main(void *arg);

        void start();

        pid_t getpid(){
            return m_thread_id;
        }

    private:
        pid_t m_thread_id {0};   //线程号
        pthread_t m_thread {0};     //线程句柄
        EventLoop * m_event_loop{NULL};   //当前io线程的eventloop对象
        sem_t m_init_semaphore;
        sem_t m_start_semaphore;
    };
} 






#endif