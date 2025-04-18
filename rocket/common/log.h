#ifndef ROCKET_COMMON_LOG_H
#define ROCKET_COMMON_LOG_H

#include  <string>
#include <queue>
#include <memory>
#include "rocket/common/config.h"
namespace rocket{


    class Logger;
    class LogEvent;

    template<typename... Args>
    std::string formatstring(const char *str,Args &&... args){
        int size=snprintf(nullptr,0,str,args...);   //先得到格式化之后的大小
        std::string result;

        result.resize(size);
        if(size>0){
            snprintf(&result[0],size+1,str,args...);
        }
        return result;

    }

    #define DEBUGLOG(str,...)\
        if(rocket::Logger::GetGloballLogger()->Get_loglevel()<=rocket::Debug) \
        { \
        std::string msg=(rocket::LogEvent(rocket::Debug)).toString()+ "[" + std::string(__FILE__) + ":" + std::to_string(__LINE__ ) +  "]\t" + \
        rocket::formatstring(str,##__VA_ARGS__); \
        rocket::Logger::GetGloballLogger()->pushLog(msg);  \
        rocket::Logger::GetGloballLogger()->log();  \
        } \

    #define INFOLOG(str,...)\
        if(rocket::Logger::GetGloballLogger()->Get_loglevel()<=rocket::Info) \
        { \
        std::string msg=(rocket::LogEvent(rocket::Info)).toString()+"[" + std::string(__FILE__) + ":" + std::to_string(__LINE__ ) +  "]\t" + \
        rocket::formatstring(str,##__VA_ARGS__);\
        rocket::Logger::GetGloballLogger()->pushLog(msg);  \
        rocket::Logger::GetGloballLogger()->log();  \
        } \

    #define ERRORLOG(str,...)\
        if(rocket::Logger::GetGloballLogger()->Get_loglevel()<=rocket::Error) \
            { \
        std::string msg=(rocket::LogEvent(rocket::Error)).toString()+"[" + std::string(__FILE__) + ":" + std::to_string(__LINE__ ) +  "]\t"+ \
        rocket::formatstring(str,##__VA_ARGS__);\
        rocket::Logger::GetGloballLogger()->pushLog(msg);  \
        rocket::Logger::GetGloballLogger()->log();  \
        } \


    enum Loglevel{
        Unkown=0,
        Debug=1,
        Info=2,
        Error=3
    };

    class Logger{
        public: 
            Logger(Loglevel log_level):m_log_level(log_level){}

            typedef std::shared_ptr<Logger> s_ptr;

            void pushLog(const std::string &msg);

            static Logger* GetGloballLogger();

            static void SetGloballLogger();

            void log();

            void Set_log_level(Loglevel level);

            Loglevel Get_loglevel()const{
                    return m_log_level;
            }

        private:

            Loglevel m_log_level;
                 
            std::queue<std::string>m_buffer;           
    };


    std::string LogLevel_tostring(Loglevel level);
    Loglevel StringtoLoglevel(const std::string& log_level);


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


        private:
            std::string m_file_name;     //文件名
            int32_t m_file_line;        //行号
            int32_t m_pid;              //线程id
            int32_t m_thread_id;        //进程id
            
            Loglevel m_level;           //事件水平
    };

    

}


#endif