#include "./log.h"
#include <sys/time.h>
#include "./utils.h"
#include <sstream>
namespace rocket{

    Logger* Logger::GetGloballLogger(){            //日志类的单例模式，只需要有一个对象
        static Logger *onece=new Logger();
        return onece;
    }


    std::string LogEvent::LogLevel_tostring(Loglevel level){
        switch (level)
        {
        case Debug:
            return "Debug";
        
        case Info:
            return "Debug";

        case Error:
            return "Debug";

        default:
            return "UNKNOWN";
        }
    }


    std::string LogEvent::toString(){

        //获取时间
        struct  timeval now_time;

        gettimeofday(&now_time,nullptr);

        struct tm now_time_t;
        localtime_r(&(now_time.tv_sec),&now_time_t);

        char buff[128];
        strftime(&buff[0],128,"%y-%m-%d:%H:%M:%S",&now_time_t);
        std::string time_str(buff);
        int ms=now_time.tv_usec*1000;
        time_str=time_str+'.'+std::to_string(ms);

        //获取进程线程ID

        m_pid=getPid();
        m_thread_id=gerThreadId();

        std::stringstream ss;
        
        ss << "[" << LogLevel_tostring(m_level) <<"]\t"
        << "[" <<time_str <<"]\t"
        << "[" <<m_pid<< ":" << m_thread_id <<"]\t"
        << "[" <<std::string(__FILE__) <<" "<<__LINE__ << "]\t";

        return ss.str();

    }

    void Logger::pushLog(const std::string &msg){
        m_buffer.push(msg);
    }

    void Logger::log(){
        while (!m_buffer.empty())
        {
            std::string msg=m_buffer.front();
            m_buffer.pop();
            printf("%s\n",msg.c_str());
        }
    
    }


}