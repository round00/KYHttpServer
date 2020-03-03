//
// Created by gjk on 2020/3/3.
//

#ifndef KYHTTPSERVER_TCPSERVER_H
#define KYHTTPSERVER_TCPSERVER_H

#include "TcpConnection.h"

typedef std::function<void(const TcpConnPtr &conn)> ConnectionCallback;
class CTcpServer{
public:
    void                init();
    void                setConnectionCb(const ConnectionCallback& cb)
    {m_connectionCb = cb;}

private:
    ConnectionCallback  m_connectionCb;
};
typedef std::shared_ptr<CTcpServer> TcpServerPtr;


#endif //KYHTTPSERVER_TCPSERVER_H
