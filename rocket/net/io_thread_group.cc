#include "rocket/net/io_thread_group.h"
#include "rocket/common/log.h"
#include "rocket/net/io_thread.h"
namespace rocket{


    IOThreadGroup::IOThreadGroup(int size):m_size(size){
        m_io_thread_group.resize(m_size);

        for(int i=0;i<m_size;i++){
            m_io_thread_group[i]=new IOThread();
        }
    }

    IOThreadGroup::~IOThreadGroup(){

    }
        
    void IOThreadGroup::start(){
        for(int i=0;i<m_io_thread_group.size();i++){
            m_io_thread_group[i]->start();
        }
    }

    IOThread *IOThreadGroup::getIOThread(){
        if(m_idex==m_io_thread_group.size()||m_idex==-1){
            m_idex=0;
        }

       return m_io_thread_group[m_idex++];

    }

    void IOThreadGroup::join(){
        for(int i=0;i<m_io_thread_group.size();i++){
            m_io_thread_group[i]->join();
        }
    }



}


