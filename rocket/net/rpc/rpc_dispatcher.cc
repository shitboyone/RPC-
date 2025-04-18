#include "rocket/net/rpc/rpc_dispatcher.h"
#include <google/protobuf/service.h>
#include "rocket/common/log.h"
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include "rocket/net/coder/tinypb_protocol.h"
#include "rocket/common/error_code.h"
#include "rocket/net/rpc/rpc_controller.h"
#include "rocket/tcp/tcp_connection.h"
namespace  rocket
{   
    static RpcDispatcher*g_rpc_dispatcher=nullptr;
    void  RpcDispatcher::dispatch(AbstractProtocol::s_ptr message,AbstractProtocol::s_ptr response,TcpConnection* connection){
         
        std::shared_ptr<TinyPBProtocol>req_protocol=std::dynamic_pointer_cast<TinyPBProtocol>(message);
        std::shared_ptr<TinyPBProtocol>rsp_protocol=std::dynamic_pointer_cast<TinyPBProtocol>(response);
        std::string method_full_name=req_protocol->m_method_name;
        std::string service_name;
        std::string method_name;
        rsp_protocol->m_msg_id=req_protocol->m_msg_id;
        rsp_protocol->m_method_name=req_protocol->m_method_name;
        if(!parseServiceFullname(method_full_name,service_name,method_name)){
            SetTinyPBError(rsp_protocol,ERROR_PARSE_SERVICE_NAME,"parse service name error");
            return ;
        }
        auto it=m_service_map.find(service_name);
        if(it==m_service_map.end()){
            SetTinyPBError(rsp_protocol,ERROR_SERVICE_NOT_FOUND,"not find service name");
            ERRORLOG("not find service name[%s]",service_name.c_str());
            return ;
        }
        service_s_ptr service=(*it).second;
        const google::protobuf::MethodDescriptor* method=service->GetDescriptor()->FindMethodByName(method_name);
        if(method==nullptr){
            SetTinyPBError(rsp_protocol,ERROR_METHOD_NOT_FOUND,"not find method name");
            ERRORLOG("not find method name[%s]",method_name.c_str());
            return ;
        }
        google::protobuf::Message* request=service->GetRequestPrototype(method).New();

        //反序列化
        if(!request->ParseFromString(req_protocol->m_pb_data)){
            SetTinyPBError(rsp_protocol,ERROR_SERVICE_NOT_FOUND,"parse request error");
            ERRORLOG("parse request message error");
            if(request!=nullptr){
                delete request;
                request=nullptr;
            }
            return ;
        }
        INFOLOG("req_id[%s],get rpc request %s",req_protocol->getReqId().c_str(),request->ShortDebugString().c_str());

        //创建响应的消息
        google::protobuf::Message *rsp_msg=service->GetResponsePrototype(method).New();
        RpcController RpcController;
        IPNetAddr::s_ptr local_addr=std::make_shared<IPNetAddr>("127.0.0.1",1234);
        RpcController.SetLocalAddr(connection->getLocalAddr());
        RpcController.SetPeerAddr(connection->getPeerAddr());
        RpcController.SetMsgId(req_protocol->getReqId());

        //执行方法，结果保存在rsp_msg中
        service->CallMethod(method, &RpcController, request,rsp_msg, nullptr);


        //序列化
        if(!rsp_msg->SerializeToString(&rsp_protocol->m_pb_data)){
            SetTinyPBError(rsp_protocol,ERROR_SERVICE_NOT_FOUND,"parse response error");
            ERRORLOG("parse response message error");
            if(request!=nullptr){
                delete request;
                request=nullptr;
            }
            if(rsp_msg!=nullptr){
                delete rsp_msg;
                rsp_msg=nullptr;
            }
            return ;
        }

        rsp_protocol->m_err_code=0;
        INFOLOG("dispatch success,req_id[%s],get rpc response[%s]",rsp_protocol->getReqId().c_str(),rsp_protocol->m_pb_data.c_str());
        delete request;
        delete rsp_msg;
        request=nullptr;
        rsp_msg=nullptr;    

    };

    void RpcDispatcher::registerService(service_s_ptr service){
        std::string service_name=service->GetDescriptor()->full_name();
        m_service_map[service_name]=service;
    }

     bool RpcDispatcher::parseServiceFullname(const std::string &full_name,std::string &service_name,std::string &method_name){
        size_t pos=full_name.find(".");
        if(pos==full_name.npos){
           ERRORLOG("parse service name error,full_name[%s]",full_name.c_str());
           return false;
        }
        service_name=full_name.substr(0,pos);
        method_name=full_name.substr(pos+1);
        INFOLOG("parse service name success,service_name[%s],method_name[%s]",service_name.c_str(),method_name.c_str());
        return  true;
    }

    void RpcDispatcher::SetTinyPBError(std::shared_ptr<TinyPBProtocol> message,int32_t err_code,const std::string err_info){
        message->m_err_code=err_code;
        message->m_err_info=err_info;
        message->m_err_info_len=err_info.length();
    }


     RpcDispatcher* RpcDispatcher::GetRpcDispatcher(){
        if(g_rpc_dispatcher){
            return g_rpc_dispatcher;
        }else{
            g_rpc_dispatcher=new RpcDispatcher();
            return g_rpc_dispatcher;
        }
     }

} // namespace  rocket
