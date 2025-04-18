#ifndef ROCKET_NET_CODER_ABSTRAC_PROTCOL_H
#define ROCKET_NET_CODER_ABSTRAC_PROTCOL_H
#include <memory>
#include "rocket/tcp/tcp_buffer.h"
namespace rocket{
    class AbstractProtocol:public std::enable_shared_from_this<AbstractProtocol>{
        public:
            typedef std::shared_ptr<AbstractProtocol>s_ptr;

            std::string getReqId(){
                return m_msg_id;
            }

            void setReqId(const std::string &req_id){
                m_msg_id=req_id;
            }

            virtual ~AbstractProtocol(){};
        public:
            std::string m_msg_id;     //请求号，唯一标识一个请求或者响应
    };
}

#endif