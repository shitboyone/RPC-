#include "rocket/tcp/fd_event_group.h"
#include "rocket/common/log.h"
#include <mutex>
namespace rocket
{
    //将fd_event再封装，fd_event池，避免fd_event反复创建消耗
    static FdEventGroup*g_fd_event_group=nullptr;

    FdEventGroup::FdEventGroup(int size):m_size(size){
        for(int i=0;i<m_size;i++){
            m_fd_group.push_back(new FdEvent(i));
        } 
    }
    FdEventGroup::~FdEventGroup(){
         for(int i=0;i<m_size;i++){
            if(m_fd_group[i]!=nullptr){
                delete m_fd_group[i];
                m_fd_group[i]=nullptr;
            }
         }   
    }      

    FdEventGroup* FdEventGroup::GetFdEVentGroup(){
        if(g_fd_event_group==nullptr){
            g_fd_event_group=new FdEventGroup(128);
        }
        return g_fd_event_group;
    }


    FdEvent*FdEventGroup::getFdEvent(int fd){
        std::unique_lock<std::mutex>loc(m_mu);
        if(fd<m_fd_group.size()){
            return m_fd_group[fd];
        }
        int new_size=fd*1.5;
        for(int i=m_fd_group.size();i<new_size;i++){
            m_fd_group.push_back(new FdEvent(i));
        }
        m_size=m_fd_group.size();
        return m_fd_group[fd];
    }
    
} // namespace rocket
