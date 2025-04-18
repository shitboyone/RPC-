#include "rocket/common/utils.h"
#include <sys/syscall.h>
#include <sys/time.h>
#include <cstring>
#include <arpa/inet.h>
namespace rocket{
    static int g_pid=0;      //设置线程，进程ID缓存，避免多次syscall
    static thread_local int g_thread_pid=0;
    pid_t getPid(){
        if(g_pid!=0){
            return g_pid;
        }
        return getpid();
    }

    pid_t getThreadId(){
       if(g_thread_pid!=0){
            return g_thread_pid;
        }
        return syscall(SYS_gettid);
    }

    int64_t getNowMs(){
        timeval val;
        memset(&val,0,sizeof(val));
        gettimeofday(&val,nullptr);
        return val.tv_sec*1000+val.tv_usec/1000;
    }
    int32_t getInt32FromNetByte(const char* buf) {
         int32_t re;
         memcpy(&re, buf, sizeof(re));
         return ntohl(re);
}

}