#ifndef ROCKET_TCP_NET_ADDR_H
#define ROCKET_TCP_NET_ADDR_H 
#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <memory>

    namespace  rocket
    {
       class NetAddr{
            public:

            typedef std::shared_ptr<NetAddr>s_ptr;

            virtual sockaddr *getSocketAddr()=0;
            
            virtual socklen_t getSocketlen()=0;

            virtual int getFamiliy()=0;
            
            virtual std::string to_string()=0;

            virtual  bool IPisinvalid()=0;
            
            virtual int getfamily()=0;

       };   


       class   IPNetAddr:public NetAddr{

        public:
            IPNetAddr(const std::string &ip,u_int16_t port);

            IPNetAddr(const std::string &addr);

            IPNetAddr(sockaddr_in addr);

            sockaddr *getSocketAddr();
            
            socklen_t getSocketlen();

            int getFamiliy();
            
            bool IPisinvalid();

            std::string to_string();

            int getfamily(); 

        private:
            std::string m_ip;
            u_int16_t m_port;
            sockaddr_in m_addr;
        
       };







    } // namespace  rocket
    

    





#endif