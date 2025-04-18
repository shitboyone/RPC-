#include "rocket/common/log.h"
#include <sys/time.h>
#include "rocket/common/utils.h"
#include <sstream>
#include <mutex>
namespace rocket{

    
    static Logger *g_logger=nullptr;
    std::mutex mu;      //缓冲区锁

    std::string LogLevel_tostring(Loglevel level){
        switch (level)
        {
        case Debug:
            return "Debug";
        
        case Info:
            return "Info";

        case Error:
            return "Error";

        default:
            return "UNKNOWN";
        }
    }


    Loglevel StringtoLoglevel(const std::string& log_level){
        if(log_level=="DEBUG"){
            return Debug;
        }
        else if(log_level=="INFO"){
            return Info;
        }
        else if(log_level=="ERROR"){
            return Error;
        }
        else{
            return Unkown;
        }
    }

    std::string LogEvent::toString(){

        //获取时间
        struct  timeval now_time;

        gettimeofday(&now_time,nullptr);

        struct tm now_time_t;
        localtime_r(&(now_time.tv_sec),&now_time_t);

        char buff[256];
        strftime(&buff[0],128,"%y-%m-%d:%H:%M:%S",&now_time_t);
        std::string time_str(buff);
        int ms=now_time.tv_usec*1000;
        time_str=time_str+'.'+std::to_string(ms);

        //获取进程线程ID

        m_pid=getPid();
        m_thread_id=getThreadId();

        std::stringstream ss;
        
        ss << "[" << LogLevel_tostring(m_level) <<"]\t"
        << "[" <<time_str <<"]\t"
        << "[" <<m_pid<< ":" << m_thread_id <<"]\t";
        // << "[" <<std::string(__FILE__) <<" "<<__LINE__ << "]\t";     //__FILE__在预处理会替换，无法实现打印调用者的文件名
        return ss.str();

    }

   

    Logger* Logger:: GetGloballLogger(){            //日志类的单例模式，只需要有一个对象
        return g_logger;
    }


    void Logger:: SetGloballLogger(){
        Loglevel global_log_level=StringtoLoglevel(Config::GetGlobalConfig()->m_log_level);
        g_logger=new Logger(global_log_level);
    }


    void Logger::pushLog(const std::string &msg){
        {
            std::unique_lock<std::mutex> lock(mu);
            m_buffer.push(msg);
        }
        
    }

    void Logger::log(){          //消费
        {
            std::unique_lock<std::mutex> lock(mu);
            while (!m_buffer.empty())
            {
                std::string msg=m_buffer.front();
                m_buffer.pop();
                printf("%s\n",msg.c_str());
            }
        }
    }

    void Logger::Set_log_level(Loglevel level){
            m_log_level=level;
    }

    

}