#include "rocket/common/log.h"
#include <string.h>
#include "rocket/tcp/tcp_buffer.h"



    namespace rocket
        {
 
                TCpBUffer::TCpBUffer(int size):m_size(size){
                    m_buffer.resize(m_size);
                }
                TCpBUffer::~TCpBUffer() {


                }
                //返回可读字节数
                int TCpBUffer:: readAble(){
                    return m_write_index-m_read_index;
                }

                //返回可写字节数
                int TCpBUffer:: writeAble(){
                    return m_buffer.size()-m_write_index;
                }

                int TCpBUffer:: readIndex(){
                    return m_read_index;
                }

                int TCpBUffer:: writeIndex(){
                    return m_write_index;
                }

                void TCpBUffer:: writeToBUffer(const char *buf,int size){
                    if(size>writeAble()){
                        //调整buffer大小
                        int new_size=(int)(1.5*(m_write_index+size));
                        resizeBuffer(new_size);
                    }
                     memcpy(&m_buffer[m_write_index],buf,size);
                     m_write_index += size; 
                     INFOLOG("writetobuffer succ");
                }

               void TCpBUffer:: readFromBuffer(std::vector<char>&re,int size){
                  if(readAble() == 0){
                    INFOLOG("no data readfrombuffer");
                    return ;
                  }
                  int read_size=readAble()>size? size:readAble();
                  std::vector<char>tmp(read_size);
                  memcpy(&tmp[0],&m_buffer[m_read_index],read_size);

                  re.swap(tmp);
                  m_read_index+=read_size;
                  adjustBuffer(); 
                  INFOLOG("readfrombuffer succ");
                }   
    
                void TCpBUffer::resizeBuffer(int new_size){
                    std::vector<char>tmp(new_size);
                    int count=std::min(new_size,readAble());
                    memcpy(&tmp[0],&m_buffer[0],count);
                    m_buffer.swap(tmp);

                    m_read_index=0;
                    m_write_index=m_read_index+count;
                     
                }
                void TCpBUffer::adjustBuffer(){
                    if(m_read_index<int(m_buffer.size())/3){
                        return;
                    }
                    INFOLOG("adjust buffering");
                    std::vector<char>buffer(m_buffer.size());
                    int count=readAble();

                    memcpy(&buffer[0],&m_buffer[m_read_index],count);
                    m_buffer.swap(buffer);

                    m_read_index=0;
                    m_write_index=m_read_index+count;
                    buffer.clear();
                }

                void TCpBUffer::moveReadIndex(int size){
                     size_t j=m_read_index+size;
                     if(j>= m_buffer.size()){
                        ERRORLOG("moveReadIndex error,invaild size %d,old read index%d ,buffer size%d",size,m_read_index,m_buffer.size());
                        return;
                     }   
                     m_read_index=j;
                     adjustBuffer();
                }

                void TCpBUffer::moveWriteIndex(int size){
                    size_t j=m_write_index+size;
                    if(j>= m_buffer.size()){
                       ERRORLOG("moveReadIndex error,invaild size %d,old read index%d ,buffer size%d",size,m_read_index,m_buffer.size());
                       return;
                    }   
                    m_write_index=j;
                    adjustBuffer();

                }

} // namespace name
