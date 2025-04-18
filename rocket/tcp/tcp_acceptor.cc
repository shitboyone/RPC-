#include "rocket/tcp/tcp_acceptor.h"
#include "rocket/common/log.h"
#include <sys/socket.h>
#include <cstring>

namespace rocket
{
 
    TcpAcceptor::TcpAcceptor(NetAddr::s_ptr local_addr):m_locall_addr(local_addr){
            if(!m_locall_addr->IPisinvalid()){
                ERRORLOG("ipdress is invaid");
                exit(0);
            }
           m_family=m_locall_addr->getFamiliy();
           
           m_listend_fd=socket(m_family,SOCK_STREAM,0);

           if(m_listend_fd==-1){
                 ERRORLOG("failed to create socket fd");
                exit(0);
           }

           int val=1;
           if(setsockopt(m_listend_fd,SOL_SOCKET,SO_REUSEADDR,&val,sizeof(val))==-1){
                ERRORLOG("setsockeopt failed");
           }

           socklen_t len=m_locall_addr->getSocketlen();
           if(bind(m_listend_fd,m_locall_addr->getSocketAddr(),len)==-1){
                ERRORLOG("falied to bind socket_fd=%d",m_listend_fd);
                exit(0);
           }

           if(listen(m_listend_fd,1000)==-1){
                ERRORLOG("failed to listen fd=%d",m_listend_fd);
                exit(0);
           }
           DEBUGLOG("ser fd=%d正在监听client",m_listend_fd);
          
    }  
    
    
    int TcpAcceptor::getListenFd(){
        return m_listend_fd;
    }

     std::pair<int,NetAddr::s_ptr> TcpAcceptor::accept(){
                if(m_family==AF_INET){
                    sockaddr_in clienaddr;
                    memset(&clienaddr,0,sizeof(clienaddr));
                    socklen_t client_len=sizeof(clienaddr);
                    //accept
                    int clientfd=::accept(m_listend_fd,reinterpret_cast<sockaddr*>(&clienaddr),&client_len);
                    if(clientfd==-1){
                        ERRORLOG("faile to accept client");
                    }
                    //IPNetAddr peer_addr(clienaddr);   //用智能指针还是局部变量
                    IPNetAddr::s_ptr peer_addr=std::make_shared<IPNetAddr>(clienaddr);
                    INFOLOG("client succ accept,ip=%s",peer_addr->to_string().c_str());
                    return std::make_pair(clientfd,peer_addr);
                }else{    //其他协议
                    return std::make_pair(-1,nullptr);
                }
           }




} // namespace rocket






