#include "rocket/tcp/net_addr.h"
#include "rocket/common/log.h"
#include <cstring>


namespace rocket
{
 
    IPNetAddr::IPNetAddr(const std::string &ip,u_int16_t port):m_ip(ip),m_port(port){
       memset(&m_addr,0,sizeof(m_addr));
       m_addr.sin_family=AF_INET;
       m_addr.sin_addr.s_addr=inet_addr(m_ip.c_str());
       m_addr.sin_port=htons(m_port);
       INFOLOG("server ip=%s,server_port=%d",m_ip.data(),m_port);
    }

    IPNetAddr::IPNetAddr(const std::string &addr){
        int i=addr.find_first_of(':');
        if(i==addr.npos){
            DEBUGLOG("address is invaild,ip=%s",addr.c_str());
            return;
        }
        m_ip=addr.substr(0,i);
        m_port=atoi(addr.substr(i+1,addr.size()-i-1).c_str());
        memset(&m_addr,0,sizeof(m_addr));
        m_addr.sin_family=AF_INET;
        m_addr.sin_addr.s_addr=inet_addr(m_ip.c_str());
        m_addr.sin_port=m_port; 
    }

    IPNetAddr::IPNetAddr(sockaddr_in addr):m_addr(addr){
        m_ip=std::string(inet_ntoa(m_addr.sin_addr));
        m_port=ntohs(m_addr.sin_port);
    }


    sockaddr *IPNetAddr::getSocketAddr(){
        return reinterpret_cast<sockaddr*>(&m_addr);
    }
    
    socklen_t IPNetAddr::getSocketlen(){
        return sizeof(m_addr);
    }

    int IPNetAddr::getFamiliy(){
        return AF_INET;
    }
    
    std::string IPNetAddr::to_string(){
        return m_ip+std::to_string(m_port);
    }

    bool IPNetAddr::IPisinvalid(){
         if(m_ip.empty()){
            return false;
        }
        if(m_port<0||m_port>65535){
            return false;
        }
        if(inet_addr(m_ip.c_str())==INADDR_NONE){
            return false;
        }
        return true; 
    } 
    
    int IPNetAddr::getfamily(){
        return m_addr.sin_family;
    }

} // namespace rocket







