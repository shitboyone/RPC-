#include <../common/log.h>
#include <iostream>
#include <thread>
#include <unordered_map>
#include "rocket/common/config.h"
#include "rocket/net/EventLoop.h"
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>  // 包含 sockaddr_in 的定义
#include <cstring>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <memory>
#include <rocket/net/timer_event.h>
#include <rocket/net/timer.h>
#include "rocket/net/io_thread.h"
#include "rocket/net/io_thread_group.h"
// #include <semaphore>
using namespace std;

int main(){

    rocket::Config::SetGlobalConfig("../conf/rocket.xml");
    rocket::Logger::SetGloballLogger();
    // //初始化Evenloop
    // rocket:: EventLoop *test_loop=new rocket::EventLoop();


    //启动一个server服务
    //第一步：创建套接字
    int ser_fd=socket(AF_INET,SOCK_STREAM,0);
    if(ser_fd==-1){
        ERRORLOG("create failed ser_fd");
    }
    //绑定端口IP
    struct sockaddr_in seraddr;
    memset(&seraddr,0,sizeof(seraddr));
    seraddr.sin_family=AF_INET;
    seraddr.sin_port=htons(12345);
    // seraddr.sin_addr.s_addr==htonl(INADDR_ANY); //指定全部IP通信
    inet_aton("127.0.0.1",&seraddr.sin_addr);
    //绑定
    if(bind(ser_fd,(sockaddr *)&seraddr,sizeof(seraddr))==-1){
        ERRORLOG("failed to bind serfd=[%d]",ser_fd);
    }
    //监听
    if(listen(ser_fd,100)==-1){
        ERRORLOG("listen failed ");
    }
    rocket::FdEvent event(ser_fd);

    //注册fd的回调函数
    event.listen(rocket::FdEvent::IN_EVENT,[ser_fd](){
        //接受新连接
        sockaddr_in peer_addr;
        socklen_t addr_len=0;
        memset(&peer_addr,0,sizeof(peer_addr));
        int clientfd=accept(ser_fd,reinterpret_cast<sockaddr *>(&peer_addr),&addr_len);

        inet_ntoa(peer_addr.sin_addr);
        DEBUGLOG("success get client[%s : %d]",inet_ntoa(peer_addr.sin_addr),ntohs(peer_addr.sin_port));
    }
    );

    //创建定时任务
    int i=0;
    rocket::TimerEvent::s_ptr timer_event = std::make_shared<rocket::TimerEvent>(
        1000,true,[&i](){
            INFOLOG("trigger time Event,count=%d",i++);
        }
    );
    //io_eventloop_group


    rocket::IOThreadGroup test_group(2);
    rocket:: EventLoop*t1=test_group.getIOThread()->getEventLoop();
    t1->addEpollEvent(&event);
    t1->addTimerEvent(timer_event);


    rocket:: EventLoop*t2=test_group.getIOThread()->getEventLoop();
    t2->addTimerEvent(timer_event);

    test_group.start();
    test_group.join();

    //IOeventloop
    // rocket::IOThread io_thread;
    // io_thread.getEventLoop()->addTimerEvent(timer_event);
    
    // io_thread.getEventLoop()->addEpollEvent(&event);

    // io_thread.start();

    // test_loop->addTimerEvent(timer_event);

    // test_loop->addEpollEvent(&event);
    // //事件循环
    // test_loop->loop();
    // io_thread.join();

    return 0;

}