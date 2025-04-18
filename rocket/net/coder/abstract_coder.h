#ifndef ROCKET_NET_CODER_ABSTRACT_CODER_H
#define ROCKET_NET_CODER_ABSTRACT_CODER_H
#include "rocket/tcp/tcp_buffer.h"
#include "rocket/net/coder/abstract_protcol.h"
#include <vector>
namespace rocket{


    class AbstractCoder{
        public:
            virtual void encode(std::vector<AbstractProtocol::s_ptr>&messages,TCpBUffer::s_ptr our_buffer)=0;         //将message对象转换为字符流，写入buffer
            virtual void decode(std::vector<AbstractProtocol::s_ptr>&out_message,TCpBUffer::s_ptr buffer)=0;        //将buffer里面的字节流转换为message对象

            virtual ~AbstractCoder(){};
    };

}

#endif