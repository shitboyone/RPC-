#include "rocket/net/rpc/rpc_channel.h"
#include "rocket/net/rpc/rpc_controller.h"
#include "rocket/common/msg_id_util.h"
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include "rocket/common/error_code.h"
#include "rocket/tcp/tcp_client.h"
#include "rocket/common/log.h"
#include "rocket/common/msg_id_util.h"
#include <memory>
#include "rocket/net/timer_event.h"
namespace  rocket
{
    void RpcChannel::Init(controller_s_ptr controller,message_s_ptr req,message_s_ptr res,closure_s_ptr done){
        if(is_init){
            return ;
        }
        m_controller=controller;
        m_request=req;
        m_response=res;
        m_closure=done;
        is_init=true;
    }

     RpcChannel::RpcChannel(IPNetAddr::s_ptr addr):m_peer_addr(addr){
            m_client=std::make_shared<TcpcClient>(m_peer_addr);
            INFOLOG("RpcChannel");
     }

    void RpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                    google::protobuf::RpcController* controller,
                    const google::protobuf::Message* request,
                    google::protobuf::Message* response,
                    google::protobuf::Closure* done){
                        //TODO    
                //1：构建消息体
                std::shared_ptr<rocket::TinyPBProtocol>req_protocol=std::make_shared<rocket::TinyPBProtocol>();
                RpcController *my_controller=dynamic_cast<RpcController*>(controller);
                if(my_controller==nullptr){
                    ERRORLOG("controller is null");
                    return ;
                }

                if(my_controller->GetMsgId().empty()){
                    req_protocol->m_msg_id=MsgIDUtil::GenMsgID();
                    my_controller->SetMsgId(req_protocol->m_msg_id);
                }else{
                    req_protocol->m_msg_id=my_controller->GetMsgId();
                }
                req_protocol->m_method_name=method->full_name();
                INFOLOG("%s method name is %s",req_protocol->getReqId().c_str(),req_protocol->m_method_name.c_str());
               
                //是否初始化
                if(!is_init){
                    std::string err_info="channel not init";
                    my_controller->SetError(ERROR_RPC_CHANNNEL_INIT,err_info);
                    ERRORLOG("Rpcchannel not init");
                    return ;
                } 


                //request序列化
                if(!request->SerializeToString(&req_protocol->m_pb_data)){
                    std::string err_info="failde to serialize";
                    my_controller->SetError(ERROR_FAILED_SERIALIZE,err_info);
                    ERRORLOG("serialize request error");
                    return ;
                }

                s_ptr channel=shared_from_this();

                m_timer_event=std::make_shared<TimerEvent>(my_controller->GetTimeout(),false,
                [my_controller,channel]()mutable{
                    my_controller->StartCancel();
                    my_controller->SetError(ERROR_RPC_CALL_TIMEOUT,"rpc call timeout"+std::to_string(my_controller->GetTimeout()));

                    if(channel->getClosure()){
                        channel->getClosure()->Run();
                    }
                    channel.reset();

                });
                
                m_client->addTimerEvent(m_timer_event);

                //2:连接
                m_client->connect([req_protocol,done,channel]()mutable{
                    channel->getTcpClient()->writeMessage(req_protocol,[req_protocol,done,channel](AbstractProtocol::s_ptr)mutable{
                        INFOLOG("%s send rpc request success,call method name [%s]",
                        req_protocol->m_msg_id.c_str(),req_protocol->m_method_name.c_str()
                        );             

                   channel->getTcpClient()->readMessage(req_protocol->m_msg_id,[done,channel](AbstractProtocol::s_ptr msg)mutable{
                         //取消超时任务
                        channel->getTimerEvent()->setCancled(true);
                        std::shared_ptr<rocket::TinyPBProtocol>rsp_protocol=std::dynamic_pointer_cast<rocket::TinyPBProtocol>(msg);
                        INFOLOG("%s  success get rpc response ,call method name %s",
                        rsp_protocol->m_msg_id.c_str(),rsp_protocol->m_method_name.c_str());
                    //反序列化
                        RpcController*my_contreoller=dynamic_cast<RpcController*>(channel->getController());
                        if(!channel->getResponse()->ParseFromString(rsp_protocol->m_pb_data)){
                            ERRORLOG("paraseries failed");
                            my_contreoller->SetError(ERROR_FAILED_SERIALIZE,"paraseries error");
                        }
                        if(my_contreoller->IsCanceled()&&channel->getClosure()){
                            channel->getClosure()->Run();
                        }
                        channel.reset();
                        });
                    });
                    
                });
                
    }

                TcpcClient * RpcChannel::getTcpClient(){
                    return m_client.get();
                }

                google::protobuf::RpcController* RpcChannel::getController(){
                        return m_controller.get();
                }
                google::protobuf::Message * RpcChannel::getRequest(){
                        return m_request.get();
                }
                google::protobuf::Message * RpcChannel::getResponse(){
                        return m_response.get();
                }
                google::protobuf::Closure * RpcChannel::getClosure(){
                        return m_closure.get();
                }


                TimerEvent::s_ptr RpcChannel::getTimerEvent(){
                    return m_timer_event;
                }


} // namespace  rocket
