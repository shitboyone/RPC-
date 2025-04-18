#ifndef ROCKET_NET_TIMER_EVENT_H
#define ROCKET_NET_TIMER_EVENT_H
#include <functional>
#include <memory>

namespace rocket{

    class TimerEvent{
        public:
            typedef std::shared_ptr<TimerEvent>s_ptr;

            TimerEvent(int interval,bool is_repeated,std::function<void()>cb);

            int64_t GetArriveTime();

            void setCancled(bool value){
                m_is_cancled=value;
            }

            bool isCancled(){
                return m_is_cancled;
            }

            bool isReapted(){
                return m_is_repeated;
            }

            void resetArriveTime();

            std::function<void ()> getCallBack(){
                return m_task;
            }

        private:
            int64_t m_arrive_time;
            int64_t m_interval;
            bool m_is_repeated{false};
            bool m_is_cancled{false};
            std::function<void()> m_task;
    };



}


#endif