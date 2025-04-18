#ifndef ROCKET_NET_CODER_STRING_CODE_H
#define ROCKET_NET_CODER_STRING_CODE_H
#include "rocket/net/coder/abstract_coder.h"
#include "rocket/net/coder/abstract_protcol.h"
namespace rocket{


    class StringProtocpl :public AbstractProtocol{
        public:
            std::string info;
    };

    class StringCoder:public AbstractCoder{
        void encode(std::vector<AbstractProtocol::s_ptr>&messages,TCpBUffer::s_ptr out_buffer){
            for(int i=0;i<messages.size();i++){
                 std::shared_ptr<StringProtocpl> msg=std::dynamic_pointer_cast<StringProtocpl>(messages[i]);
                 out_buffer->writeToBUffer(msg->info.c_str(),msg->info.length());
            }
           
        }

        //将message对象转换为字符流，写入buffer
        void decode(std::vector<AbstractProtocol::s_ptr>&out_message,TCpBUffer::s_ptr buffer){
              std::vector<char>re;  
              buffer->readFromBuffer(re,buffer->readAble());
              std::string info;
              for(int i=0;i<re.size();++i){
                info +=re[i];
              }
             std::shared_ptr<StringProtocpl>msg=std::make_shared<StringProtocpl>();
              msg->info=info;
              msg->setReqId("123456");
              out_message.push_back(msg);
        }

    };

 }
#endif