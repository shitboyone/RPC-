#include "timer_event.h"
#include "rocket/common/log.h"
#include "rocket/common/utils.h"
namespace rocket{

    TimerEvent::TimerEvent(int interval,bool is_repeated,std::function<void()>cb):m_interval(interval),m_is_repeated(is_repeated),m_task(cb){

        m_arrive_time=rocket::getNowMs()+m_interval;

        DEBUGLOG("success create time out event,will excute at [%lld]",m_arrive_time);

        resetArriveTime();
    }

    int64_t TimerEvent::GetArriveTime(){
        return m_arrive_time;
    }
    
    void TimerEvent::resetArriveTime(){
        m_arrive_time=getNowMs()+m_interval;
        DEBUGLOG("success create time out event,will excute at [%lld]",m_arrive_time);
    }
    

}