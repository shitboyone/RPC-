#include "./utils.h"
#include <sys/syscall.h>
namespace rocket{
    static int g_pid=0;      //设置线程，进程ID缓存，避免多次syscall
    static thread_local int g_thread_pid=0;
    pid_t getPid(){
        if(g_pid!=0){
            return g_pid;
        }
        return getpid();
    }
    pid_t gerThreadId(){
       if(g_thread_pid!=0){
            return g_thread_pid;
        }
        return syscall(SYS_gettid);
    }
}