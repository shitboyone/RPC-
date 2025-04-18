#include "rocket/net/io_thread.h"
#include "rocket/common/log.h"
#include <pthread.h>
#include "rocket/common/utils.h"
#include <semaphore.h>
#include <assert.h>
#include "rocket/common/log.h"
namespace rocket
{
    
    IOThread::IOThread(){

        int rt=sem_init(&m_init_semaphore,0,0);
        assert(rt==0);

        rt=sem_init(&m_start_semaphore,0,0);
        assert(rt==0);

        pthread_create(&m_thread,NULL,&IOThread::Main,this);

        //wait 等到Main函数初始化完成
        rt=sem_wait(&m_init_semaphore);
        
        DEBUGLOG("IOThreaad create succ,thread id=%d",m_thread_id);

    }



    void* IOThread::Main(void *arg){
        IOThread *thread=static_cast<IOThread*>(arg);
        thread->m_event_loop=new EventLoop();
        thread->m_thread_id=getThreadId();

        //这里是异步的，需要同步，等待初始化完成才能开启loop

        sem_post(&thread->m_init_semaphore);

        //等待自动启动
        DEBUGLOG("IOthread=%d wiat start loop",thread->m_thread_id);

        sem_wait(&thread->m_start_semaphore);
        DEBUGLOG("IOthread=%d  start loop",thread->m_thread_id);
        thread->m_event_loop->loop();

        DEBUGLOG("IOthread=%d  end loop",thread->m_thread_id);

        return nullptr;
     }



     void IOThread::join(){
        pthread_join(m_thread,nullptr);
     }

     void IOThread::start(){
        sem_post(&m_start_semaphore);
     }


     IOThread::~IOThread(){
        m_event_loop->stop();
        sem_destroy(&m_init_semaphore);
        sem_destroy(&m_start_semaphore);
        pthread_join(m_thread,NULL);

        if(m_event_loop){
            delete m_event_loop;
            m_event_loop=nullptr;
        }
     }


     EventLoop * IOThread::getEventLoop(){

        return m_event_loop;
     }


} // namespace rocket
