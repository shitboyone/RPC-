#include "rocket/tcp/tcp_server.h"
#include <memory>
#include <cstring>
#include <unistd.h>
#include "rocket/net/coder/abstract_protcol.h"
#include "rocket/tcp/net_addr.h"
#include "rocket/tcp/tcp_client.h"
#include "rocket/net/coder/abstract_protcol.h"
#include "rocket/net/coder/string_code.h"
#include "rocket/net/rpc/rpc_controller.h"
#include "rocket/order/order.pb.h"

class OrderImpl :public Order{
    public:
         virtual void makeOrder(google::protobuf::RpcController* controller,
                       const ::makeOrdererRequest* request,
                       ::makeOrderResponse* response,
                       ::google::protobuf::Closure* done){
                    
                if(request->price()<10){
                    response->set_ret_code(-1);
                    response->set_res_info("short balancd");
                    return ;
                }
                response->set_order_id("2025-4-15");
            }
};
void test(){

    rocket::Config::SetGlobalConfig("../conf/rocket.xml");
    rocket::Logger::SetGloballLogger();
    rocket::IPNetAddr::s_ptr addr=std::make_shared<rocket::IPNetAddr>("127.0.0.1",12345);
    DEBUGLOG("succ create addr=%s",addr->to_string().c_str());
    rocket::RpcDispatcher::GetRpcDispatcher()->registerService(std::make_shared<OrderImpl>());
    rocket::TcpServer ser(addr);
    ser.start();
}


void test_conncetion(){
    int fd=socket(AF_INET,SOCK_STREAM,0);
    if(fd<0){
        DEBUGLOG("faild create fd");
        exit(0);
    }
    sockaddr_in server_addr;
    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family=AF_INET;
    inet_aton("127.0.0.1",&server_addr.sin_addr);

    int rt=connect(fd,reinterpret_cast<sockaddr *>(&server_addr),sizeof(server_addr));
    
    std::string msg="hellorocket";

    rt=write(fd,msg.c_str(),msg.length());

    DEBUGLOG("succ wirte %d bytes,%s ,",rt, msg.c_str());

    char buff[100];
    rt=read(fd,buff,100);
    DEBUGLOG("succ read %d bytes,%s",rt,std::string(buff).c_str());
   
}


void test_tcp_client(){
    rocket::IPNetAddr::s_ptr addr=std::make_shared<rocket::IPNetAddr>("127.0.0.1",123456);
    rocket::TcpcClient client(addr);
    client.connect([addr,&client](){
         DEBUGLOG("connect to [%s] sucess ",addr->to_string().c_str());
         std::shared_ptr<rocket::TinyPBProtocol>message=std::make_shared<rocket::TinyPBProtocol>();
         message->setReqId("123456789");
         message->m_pb_data="hello rocket";

         client.writeMessage(message,[](rocket::AbstractProtocol::s_ptr msg_ptr){
            DEBUGLOG("send message success");
         });
         client.readMessage("123456789",[](rocket::AbstractProtocol::s_ptr msg_ptr){
         std::shared_ptr<rocket::TinyPBProtocol>message=
         std::dynamic_pointer_cast<rocket::TinyPBProtocol>(msg_ptr);
         DEBUGLOG("req_id[%s],get respose %s",message->getReqId().c_str(),message->m_pb_data.c_str());
        });
    });
}


int main(){
    
    test();

}