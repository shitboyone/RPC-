#ifndef ROCKET_MET_RPC_RPC_CHANNEL_H
#define ROCKET_MET_RPC_RPC_CHANNEL_H
#include <google/protobuf/service.h>
#include "rocket/tcp/net_addr.h"
#include "rocket/net/coder/tinypb_protocol.h"
#include "rocket/common/log.h"
#include "rocket/net/timer_event.h"
#include "rocket/tcp/tcp_client.h"
    namespace rocket{
        class RpcChannel:public google::protobuf::RpcChannel ,
        public std::enable_shared_from_this<RpcChannel>{
            public:
                typedef std::shared_ptr<RpcChannel>s_ptr;
                typedef std::shared_ptr<google::protobuf::RpcController> controller_s_ptr;
                typedef std::shared_ptr<google::protobuf::Message> message_s_ptr;
                typedef std::shared_ptr<google::protobuf::Closure>closure_s_ptr;

                RpcChannel(IPNetAddr::s_ptr addr);

                void Init(controller_s_ptr controller,message_s_ptr req,message_s_ptr res,closure_s_ptr done);

                void CallMethod(const google::protobuf::MethodDescriptor* method,
                    google::protobuf::RpcController* controller,
                    const google::protobuf::Message* request,
                    google::protobuf::Message* response,
                    google::protobuf::Closure* done);
                
                    TimerEvent::s_ptr getTimerEvent();

                TcpcClient *getTcpClient();    

                ~RpcChannel(){
                    INFOLOG("~RpcChannel");
                }

                google::protobuf::RpcController *getController();
                google::protobuf::Message *getRequest();
                google::protobuf::Message *getResponse();
                google::protobuf::Closure *getClosure();
                

            private:
                TcpcClient::s_ptr m_client;
                NetAddr::s_ptr m_peer_addr {nullptr};
                NetAddr::s_ptr m_local_addr {nullptr};
                controller_s_ptr m_controller{nullptr};
                message_s_ptr m_message{nullptr};
                message_s_ptr m_request{nullptr};
                message_s_ptr m_response{nullptr};
                closure_s_ptr m_closure {nullptr};
                bool is_init {false};

                TimerEvent::s_ptr m_timer_event {nullptr};
        };

    }




#endif

