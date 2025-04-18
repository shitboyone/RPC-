#include "rocket/net/timer.h"
#include "rocket/common/log.h"
#include "rocket/net/timer_event.h"
#include "rocket/common/utils.h"
 #include <sys/timerfd.h>
 #include "rocket/common/log.h"
 #include <vector>
 #include <cstring>
 #include <mutex>
namespace rocket{

    Timer::Timer(){

        m_fd=timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
        DEBUGLOG("time fd=[%d]",m_fd);

        //fd可读事件放到eventloop上监听
        listen(FdEvent::IN_EVENT,std::bind(&Timer::onTimer,this));
        INFOLOG("timer回调函数注册成功,fd=%d",m_fd);
    }

    void Timer::resetArriveTime(){
        std::unique_lock<std::mutex>lock(m_mu);
        auto tmp=m_pending_event;
        lock.unlock();
        if(tmp.empty()){
            return ;
        }

        auto it=tmp.begin();
        int64_t now=getNowMs();
        int64_t interval=0;
        if(it->second->GetArriveTime()>now){    //还没有超时
            interval = it->second->GetArriveTime()-now;
        }else{     //已经超时，立即执行
            interval=100;
        }

        timespec ts;
        memset(&ts,0,sizeof(ts));
        ts.tv_sec=interval/1000;
        ts.tv_nsec=(interval%1000)*1000000;

        itimerspec value;
        memset(&value,0,sizeof(value));
        value.it_value=ts;

        int rt=timerfd_settime(m_fd,0,&value,NULL);

        if(rt!=0){
            ERRORLOG("timerfd_setting error");
        }
            DEBUGLOG("timer reset to %ld",now+interval);
       
       
    }

    void Timer::addTimerEvent(TimerEvent::s_ptr event){

        bool is_reset_timefd=false;
        std::unique_lock<std::mutex>lock(m_mu);
        if(m_pending_event.empty()){
            is_reset_timefd=true;
        }else{
            auto it=m_pending_event.begin();    //当前最小的定时任务
            if((*it).second->GetArriveTime()>event->GetArriveTime()){      //当前到的任务时间更短
                is_reset_timefd=true; 
            }
        }
        m_pending_event.emplace(event->GetArriveTime(),event);
        lock.unlock();

        if(is_reset_timefd){
            resetArriveTime();
        }
        INFOLOG("create timerEvent succ,arrivetime is %ld",event->GetArriveTime());

    }

    void Timer::delteTimerEvent(TimerEvent::s_ptr event){

        event->setCancled(true);
        std::unique_lock<std::mutex>lock(m_mu);

        auto begin=m_pending_event.lower_bound(event->GetArriveTime());
        auto end=m_pending_event.upper_bound(event->GetArriveTime());

        auto it=begin;
        for(;it!=end;it++){
            if(it->second==event) break;
        }

        if(it!=end){
            m_pending_event.erase(it);
        }
        lock.unlock();
        DEBUGLOG("succ delete TimerEvent at arrive time %ld",event->GetArriveTime());

    }

    void Timer::onTimer(){  //当发生了IO事件后，eventloop会执行这个回调函数
        
        INFOLOG("timer trigger happend");
        //处理缓冲区数据，防止下一次继续触发可读事件
        char buf[8];
        while(1){
            if((read(m_fd,buf,8)==-1)&&errno==EAGAIN){
                break;
            }
        }

        //执行定时任务
        int64_t now=getNowMs();
        std::vector<TimerEvent::s_ptr>tmp;
        std::vector<std::pair<int64_t,std::function<void()>>> tasks;

        std::unique_lock<std::mutex>lock(m_mu);
        
        auto it= m_pending_event.begin();

        for(;it!=m_pending_event.end();it++){
            if((*it).first<=now){
                if(!(*it).second->isCancled()){
                    tmp.push_back((*it).second);            
                    tasks.push_back(std::make_pair((*it).second->GetArriveTime(),(*it).second->getCallBack()));
               }
            }else{
                break;
            }
        }
        
        m_pending_event.erase(m_pending_event.begin(),it);
        lock.unlock();
        
        //将重复的Event再次添加进去
        for(auto i =tmp.begin();i!=tmp.end();++i){
            if((*i)->isReapted()){
                //调整arriveTime
                (*i)->resetArriveTime();
                addTimerEvent(*i);
            }
        }

        resetArriveTime();

        //执行回调函数
        for(auto i:tasks){
            if(i.second){
                i.second();
            }
        }
    }         



}