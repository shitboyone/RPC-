#ifndef ROCKET_NET_WAKEUP_FD_EVENT_H
#define ROCKET_NET_WAKEUP_FD_EVENT_H

#include "fd_event.h"

namespace rocket{

    class WakeUpEevent  :public FdEvent{
        public:
            WakeUpEevent(int fd); 

            virtual ~WakeUpEevent(){}

        void wakeup();

        private:

    };



}

#endif