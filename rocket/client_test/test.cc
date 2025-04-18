#include <memory>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
// #include "rocket/common/log.h"
 #include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "rocket/tcp/tcp_server.h"
#include <memory>
#include <cstring>
#include <unistd.h>
#include "rocket/net/coder/abstract_protcol.h"
#include "rocket/tcp/net_addr.h"
#include "rocket/tcp/tcp_client.h"
#include "rocket/net/coder/abstract_protcol.h"
#include "rocket/net/coder/string_code.h"
#include "rocket/net/coder/tinypb_protocol.h"
#include "rocket/net/coder/tinypb_coder.h"
#include "rocket/order/order.pb.h"
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include "rocket/net/rpc/rpc_channel.h"
#include "rocket/net/rpc/rpc_controller.h"
#include "rocket/net/rpc/rpc_closure.h"
#include "rocket/net/rpc/rpc_channel.h"
void test_tcp_client(){
    //初始化日志对象
    rocket::Config::SetGlobalConfig("../conf/rocket.xml");
    rocket::Logger::SetGloballLogger();

    //初始化IP地址
    rocket::IPNetAddr::s_ptr addr=std::make_shared<rocket::IPNetAddr>("127.0.0.1",12345);
    rocket::TcpcClient client(addr);
    DEBUGLOG("client succ create");

    //连接客户端，并且指定成功的回调函数
     client.connect([addr,&client](){
         DEBUGLOG("connect to [%s] sucess ",addr->to_string().c_str());
         std::shared_ptr<rocket::TinyPBProtocol>message=std::make_shared<rocket::TinyPBProtocol>();
         message->setReqId("123456789");
        //  message->m_pb_data="hello rocket";

        makeOrdererRequest request;
        request.set_price(100);
        request.set_goods("apple");

        if(!request.SerializeToString(&message->m_pb_data)){
            DEBUGLOG("serialize request error");
            return ;
        }
        message->m_method_name="Order.makeOrder";
         client.writeMessage(message,[](rocket::AbstractProtocol::s_ptr msg_ptr){
            DEBUGLOG("send message success");
         });
         client.readMessage("123456789",[](rocket::AbstractProtocol::s_ptr msg_ptr){
         std::shared_ptr<rocket::TinyPBProtocol>message=
         std::dynamic_pointer_cast<rocket::TinyPBProtocol>(msg_ptr);
         DEBUGLOG("req_id[%s],get respose %s",message->getReqId().c_str(),message->m_pb_data.c_str());

         makeOrderResponse response;
         if(!response.ParseFromString(message->m_pb_data)){
                DEBUGLOG("parse response error");
                return ;
         }
         DEBUGLOG("response order_id=%s",response.order_id().c_str());  
        });
    });
}



void test_rpc_channel(){
    rocket::IPNetAddr::s_ptr addr=std::make_shared<rocket::IPNetAddr>("127.0.0.1",12345);
    std::shared_ptr<rocket::RpcChannel>channel =std::make_shared<rocket::RpcChannel>(addr);

    std::shared_ptr<makeOrdererRequest>request=std::make_shared<makeOrdererRequest>();
    request->set_price(100);
    request->set_goods("apple");

    std::shared_ptr<makeOrderResponse>response=std::make_shared<makeOrderResponse>();

    std::shared_ptr<rocket::RpcController>controller=std::make_shared<rocket::RpcController>();
    controller->SetMsgId("99999");

    std::shared_ptr<rocket::RpcClosure>closure=std::make_shared<rocket::RpcClosure>(
        [request,response,channel,controller]()mutable{
            if(controller->GetErrorCode()==0){
                INFOLOG("call rpc succ ,request[%s],reponse[%s]",request->ShortDebugString().c_str(),response->ShortDebugString().c_str());
                //执行业务逻辑
            }else{
                ERRORLOG("call rpc failed ,request [%s],error code [%d],error info[%s]",
                  request->ShortDebugString().c_str(),
                  controller->GetErrorCode(),
                  controller->GetErrorInfo().c_str()
                );
            }
            INFOLOG("now exit event loop");
            channel->getTcpClient()->stop();
            channel.reset();
        }
    );
    channel->Init(controller,request,response,closure);

    Order_Stub stub(channel.get());

    stub.makeOrder(controller.get(),request.get(),response.get(),closure.get());

}

int main(){
 //初始化日志对象
    rocket::Config::SetGlobalConfig("../conf/rocket.xml");
    rocket::Logger::SetGloballLogger();
    test_rpc_channel();
}