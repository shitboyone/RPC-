#include "rocket/net/fd_event.h"
#include "rocket/net/wakeup_fd_event.h"
#include <unistd.h>
#include "rocket/common/log.h"
namespace rocket{

    WakeUpEevent::WakeUpEevent(int fd):FdEvent(fd){
    }

    void WakeUpEevent::wakeup(){
        char buff[8]={'a'};
        int rt=write(m_fd,buff,8);
        if(rt!=8){
            ERRORLOG("write to wakeup fd less than 8 bytes,fd=[%d]",m_fd);
        }
        INFOLOG("succ read 8 bytes");
    }

}