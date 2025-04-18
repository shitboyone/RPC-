#ifndef ROCKET_TCP_FD_EVENT_GROUP_H
#define ROCKET_TCP_FD_EVENT_GROUP_H 
#include "rocket/net/fd_event.h"
#include "vector"
#include "mutex"
namespace  rocket
{
    class FdEventGroup{
        public: 
            FdEventGroup(int size);
            ~FdEventGroup();       
            FdEvent *getFdEvent(int fd);

        public:
            static FdEventGroup*GetFdEVentGroup();

        private:
            int m_size {0};
            std::vector<FdEvent *>m_fd_group;            
            std::mutex m_mu;
    };

    
} // namespace  rocket





#endif