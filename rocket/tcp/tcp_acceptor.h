#ifndef ROCKET_TCP_TCP_ACCEPTOR_H
#define ROCKET_TCP_TCP_ACCEPTOR_H 
#include "rocket/tcp/net_addr.h"
#include <memory>
    namespace rocket
 {
    class TcpAcceptor{
    public:
        typedef std::shared_ptr<TcpAcceptor>s_ptr; 
        TcpAcceptor(NetAddr::s_ptr local_addr);
        int getListenFd();
        ~TcpAcceptor(){}
        std::pair<int,NetAddr::s_ptr> accept();

    private:
        int m_family{-1};
        int m_listend_fd {-1};
        NetAddr::s_ptr m_locall_addr;
    };        
        




        


} // namespace rocket
    










#endif