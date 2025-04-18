#include <rocket/common/log.h>
#include <iostream>
#include <thread>
#include <unordered_map>
#include "rocket/common/config.h"
// #include <semaphore>
using namespace std;

void test1(){
    DEBUGLOG("this is Dubug %s\n","fun");
    // INFOLOG("this is Info%s\n","fun");
    // ERRORLOG("this is ERRORLOG%s\n","fun")
}

// void test2(){
//     DEBUGLOG("this is tow question");
// }


class test{
    public:
    int a=1;
};


int main(){

    rocket::Config::SetGlobalConfig("../conf/rocket.xml");
    rocket::Logger::SetGloballLogger();

    std::thread t1(test1);
    std::thread t2(test1);
    std::thread t3(test1);
    std::thread t4(test1);
    std::thread t5(test1);
    

    // INFOLOG("this is Dubug %s\n","fun");
    // INFOLOG("this is Info%s\n","fun");
    // // std::thread t2(test2); 
    // std::cout<<"thread create success"<<std::endl;
    t1.join(); 
    t2.join(); 
    t3.join(); 
    t4.join(); 
    t5.join(); 
    // // t2.join();
}