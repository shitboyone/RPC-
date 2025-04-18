#ifndef ROCKET_NET_RPC_RPC_DISPATCHER_H
#define ROCKET_NET_RPC_RPC_DISPATCHER_H
#include "rocket/net/coder/abstract_protcol.h"
#include <map>
#include<google/protobuf/service.h>
#include "rocket/net/coder/tinypb_protocol.h"
namespace rocket
{   
    class TcpConnection;
    class RpcDispatcher
    {
        public:
            typedef std::shared_ptr<google::protobuf::Service> service_s_ptr;
            typedef std::shared_ptr<RpcDispatcher> s_ptr;
            RpcDispatcher(){};
            ~RpcDispatcher(){};

            //将请求的消息进行分发
            void dispatch(AbstractProtocol::s_ptr message,AbstractProtocol::s_ptr response,TcpConnection* connection);


            void registerService(service_s_ptr service);

            bool parseServiceFullname(const std::string &full_name,std::string &service_name,std::string &method_name);

            void SetTinyPBError(std::shared_ptr<TinyPBProtocol> message,int32_t err_code,const std::string err_info);

            static RpcDispatcher *GetRpcDispatcher();
            

        private:    
          std::map<std::string,service_s_ptr>m_service_map;        


    }; // namespace rocket

}
   

#endif