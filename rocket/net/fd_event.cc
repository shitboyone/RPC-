#include  "rocket/net/fd_event.h"
#include <cstring>
#include <fcntl.h>
namespace  rocket{

    FdEvent::FdEvent(int fd):m_fd(fd){
        memset(&m_listen_events, 0, sizeof(m_listen_events));
    }

    FdEvent::FdEvent(){
        memset(&m_listen_events, 0, sizeof(m_listen_events));
    }



    std::function<void ()> FdEvent::handle(TriggerEvent event_type){
        if(event_type==TriggerEvent::IN_EVENT){
            return m_read_callback;
        }
        else{
            return  m_write_callback;
        }
    }

 

    void FdEvent::listen(TriggerEvent event_type,std::function<void ()>callback){

        if(event_type==TriggerEvent::IN_EVENT){
            m_listen_events.events|=EPOLLIN;
            m_read_callback=callback;
        }else{
            m_listen_events.events|=EPOLLOUT;
            m_write_callback=callback;
        }


        //显示的将fdevent赋值给epollevent->data->ptr,事件触发之后返回当前fdevent对象
        m_listen_events.data.ptr=this;

    }

    void FdEvent::setNonBlock(){
        
        int flag=fcntl(m_fd,F_GETFL,0);
        if(flag & O_NONBLOCK){
            return ;
        }
        fcntl(m_fd,F_SETFL,flag|O_NONBLOCK);
    }

      void FdEvent::cancel(TriggerEvent event_type){
          if(event_type==TriggerEvent::IN_EVENT){
              m_listen_events.events &=(~EPOLLIN);
          }
          else{
            m_listen_events.events &=(~EPOLLOUT);
          }
      }

}
