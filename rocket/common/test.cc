#include <./log.h>
#include <iostream>
#include <thread>
#include <unordered_map>
// #include <semaphore>
using namespace std;

void test1(){
    DEBUGLOG("this is one question in %s\n","fun");
}

// void test2(){
//     DEBUGLOG("this is tow question");
// }

class P{

    public:
    const static int a=1;
};


int main(){
    P p;
    unordered_map<char,int>meo;
    meo['a']++;
    cout<<meo['a']<<endl;
    // std::thread t1(test1);
    // // std::thread t2(test2); 
    // std::cout<<"thread create success"<<std::endl;
    // t1.join();
    // // t2.join();
}