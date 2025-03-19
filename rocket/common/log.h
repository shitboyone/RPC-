#ifndef ROCKET_COMMON_LOG_H
#define ROCKET_COMMON_LOG_H

#include  <string>
#include <queue>
#include <memory>
namespace rocket{


    class Logger;
    class LogEvent;

    template<typename... Args>
    std::string formatstring(const char *str,Args &&... args){
        int size=snprintf(nullptr,0,str,args...);   //先得到格式化之后的大小
        std::string result;

        result.resize(size);
        if(size>0){
            snprintf(&result[0],size,str,args...);
        }
        return result;
        
    }

    #define DEBUGLOG(str,...)\
        std::string msg=(new rocket::LogEvent(rocket::Debug))->toString()+rocket::formatstring(str,##__VA_ARGS__);\
        rocket::Logger::GetGloballLogger()->pushLog(msg);  \
        rocket::Logger::GetGloballLogger()->log();  \


    enum Loglevel{
        Debug=1,
        Info=2,
        Error=3
    };

    class Logger{
        public:
            typedef std::shared_ptr<Logger> s_ptr;

            void pushLog(const std::string &msg);

            static Logger* GetGloballLogger();

            void log();

        private:
            Logger(){}      //构造私有化
            std::queue<std::string>m_buffer;           
    };




    class LogEvent{
        public:

            LogEvent(Loglevel level):m_level(level){}

            std::string getFileName(){
                return m_file_name;
            }

            Loglevel getLogLevel(){
                return m_level;
            }

            std::string toString();

            std::string LogLevel_tostring(Loglevel level);

        private:
            std::string m_file_name;     //文件名
            int32_t m_file_line;        //行号
            int32_t m_pid;              //线程id
            int32_t m_thread_id;        //进程id
            
            Loglevel m_level;           //事件水平
    };

    

}


#endif