#include "rocket/net/EventLoop.h"
#include <sys/socket.h>
#include "rocket/common/log.h"
#include "rocket/common/utils.h"
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <string.h>
#include <mutex>
#include "rocket/net/fd_event.h"
#include "rocket/net/wakeup_fd_event.h"

#define ADD_TO_EPOLL() \
    auto it=m_listend_fds.find(event->getFD()); \
    int op=EPOLL_CTL_ADD; \
    if(it !=m_listend_fds.end()){  \
        op=EPOLL_CTL_MOD;  \
    } \
    epoll_event tmp=event->getEpollEvent(); \
    int rt=epoll_ctl(m_epoll_fd,op,event->getFD(),&tmp); \
    if(rt==-1){ \
        ERRORLOG("epoll wait error,error=%s",errno); \
    }\
    m_listend_fds.insert(event->getFD());  \
    INFOLOG("succ ADD fd=%d\n",event->getFD()); \

#define DELETE_TO_EPOLL() \
    auto it=m_listend_fds.find(event->getFD()); \
    if(it == m_listend_fds.end()){ \
        return ; \
    } \
    int op = EPOLL_CTL_DEL; \
    epoll_event tmp=event->getEpollEvent(); \
    int rt=epoll_ctl(m_epoll_fd,op,event->getFD(),&tmp);\
    if(rt==-1){ \
        ERRORLOG("failed to epoll_ctl when delete fd,errno=%s\n",errno);    } \
        m_listend_fds.erase(event->getFD()); \
    INFOLOG("succ delete fd=%d\n",event->getFD()); \

namespace rocket{

    static thread_local EventLoop *t_current_eventloop=nullptr;
    static int g_epoll_max_timeout=10000;
    static int g_epoll_max_event=10000;

    EventLoop::EventLoop(){
        //当前线程已经创建无需创建
        if(t_current_eventloop!=nullptr){
            ERRORLOG("failed to create event loop,this thread has create ");
            exit(0);
        }
        m_pid=getThreadId();

        //创建epoll句柄
        m_epoll_fd=epoll_create(10);
        if(m_epoll_fd==-1){
            ERRORLOG("failed to create event loop,epoll_create error,error info [%d]",errno);
            exit(0);
        }
        //添加wakeupfd到epoll中
        initWakeUp();
        iniTimer();

        //成员变量完成初始化，this安全
        t_current_eventloop=this;

    }




    void EventLoop::loop(){

        while(!m_stop_flag){
           //先执行任务，在监听epoll     
           //使用swap交换数据，清空m_pending_tasks
           std::queue<std::function<void()>>tmp_tasks;
            {
                std::unique_lock<std::mutex>lock(m_mu);
                
                m_pending_tasks.swap(tmp_tasks);
            }
            
            while(!tmp_tasks.empty()){
                std::function<void ()>cb=tmp_tasks.front();
                tmp_tasks.pop();
                INFOLOG("task event succ op");
                if(cb!=nullptr){
                    cb();
                }
            }

            int timeout=g_epoll_max_timeout;

            epoll_event result_events[g_epoll_max_event];

            int rt=epoll_wait(m_epoll_fd,result_events,g_epoll_max_event,timeout);

            if(rt<0){
                // ERRORLOG("epoll wait error,error=%s",errno);
            }else{               //取出事件执行

                for(int i=0;i<rt;++i){
                    epoll_event trigger_vent=result_events[i];
                    FdEvent *fd_event=static_cast<FdEvent *>(trigger_vent.data.ptr);
                    INFOLOG("fd=%d的事件到达",fd_event->getFD());
                    if(fd_event==nullptr){
                        continue;
                    }        
                    if(trigger_vent.events & EPOLLIN){
                        INFOLOG("add EPOLLIN");
                        addTask(fd_event->handle(FdEvent::IN_EVENT));
                    }
                    if(trigger_vent.events & EPOLLOUT){
                        INFOLOG("add EPOLLCOUT");
                        addTask(fd_event->handle(FdEvent::OUT_EVENT));
                    }
                }
                
            }

        }
    }


    void EventLoop::wakeup(){       //定时唤醒epoll线程
        INFOLOG("wakeup");
        m_wake_fd_event->wakeup();
    }

    void EventLoop::stop(){
        m_stop_flag=true;
    }

    void EventLoop::dealwakeup(){


    }

    void EventLoop::addEpollEvent(FdEvent *event ){
        if(isTnLoopThread()){
            // INFOLOG("here %d,",event->getFD());
            ADD_TO_EPOLL();
        }
        else{
            // INFOLOG("here %d,",event->getFD());  
            auto cb=[this,event](){
                ADD_TO_EPOLL();
            };
            addTask(cb,true);
            INFOLOG("task add succ event=%d",event->getFD());
        }
    }


    void EventLoop::addTimerEvent(TimerEvent::s_ptr event){
        m_timer->addTimerEvent(event);
    }


    void EventLoop::deleteEpollEvent(FdEvent *event ){
        if(isTnLoopThread()){        //event
            DELETE_TO_EPOLL();
        }
        else{
            auto cb=[this,event](){
                DELETE_TO_EPOLL();
            };
            addTask(cb,true);
        }
    }


    void EventLoop::addTask(std::function<void ()>cb,bool iswakeup /*=false*/){
        {
            std::unique_lock<std::mutex>locl(m_mu);
            m_pending_tasks.push(cb);
        }
        if(iswakeup){
            wakeup();
        }
    }


    bool EventLoop::isTnLoopThread(){
        return getThreadId()==m_pid;
    }



    void EventLoop::initWakeUp(){
        m_wakeup_fd=eventfd(0,EFD_NONBLOCK);

        if(m_wakeup_fd<=0){
            ERRORLOG("failed to create event loop,epoll_create error,error info [%d]",errno);
            exit(0);
        }
        m_wake_fd_event=new WakeUpEevent(m_wakeup_fd);

        m_wake_fd_event->listen(FdEvent::IN_EVENT,[this](){
            char buff[8];
            while(read(m_wakeup_fd,buff,8)!=-1&&errno!=EAGAIN){

            }
            DEBUGLOG("read full bytes from wakeup fd[%d]",m_wakeup_fd);
        });
        addEpollEvent(m_wake_fd_event);
        INFOLOG("succ add wakeup_fd=%d",m_wakeup_fd);
    }


    void EventLoop::iniTimer(){
        m_timer=new Timer();
        addEpollEvent(m_timer);
    }

    EventLoop *EventLoop::getCurrentEventLoop(){
        if(t_current_eventloop){
            return t_current_eventloop;
        }
        t_current_eventloop=new EventLoop();
        return t_current_eventloop;
    }

    bool EventLoop::Islooping(){
        return is_looping;
    }


    EventLoop::~EventLoop(){
        close(m_epoll_fd);
        if(m_wake_fd_event){
            delete m_wake_fd_event;
            m_wake_fd_event=nullptr;

            delete m_timer;
            m_timer=nullptr;

        }
    }



}