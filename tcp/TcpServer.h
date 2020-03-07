//
// Created by gjk on 2020/3/3.
//

#ifndef KYHTTPSERVER_TCPSERVER_H
#define KYHTTPSERVER_TCPSERVER_H

#include <vector>
#include "TcpConnection.h"

class CEventLoop;
typedef std::function<void(const TcpConnPtr &conn)> ConnectionCallback;
class CTcpServer{
public:
    explicit CTcpServer(int port, int threadCount=2);
    ~CTcpServer();
    bool                init();
    void                loop();

    void                onAccept(CEvent* event);

    void                setConnectionCb(const ConnectionCallback& cb)
    {m_connectionCb = cb;}
    ConnectionCallback  getConnectionCb(){return m_connectionCb;}

    void                setDisConnectCb(const ConnectionCallback& cb)
    {m_disConnectionCb = cb;}
    ConnectionCallback  getDisConnectionCb(){return m_disConnectionCb;}

private:

    int                 m_listenPort;
    //IO事件循环的个数
    int                 m_threadCount;
    CEvent*             m_acceptEvent;
    CEventLoop*         m_baseLoop;
    //IO 事件循环
    std::vector<CEventLoop*>
                        m_ioLoops;
    //在分发IO事件给ioLoops时，该选择哪个loop，尽量使得每个loop都负载均衡
    int                 m_nextLoop;


    ConnectionCallback  m_connectionCb;
    ConnectionCallback  m_disConnectionCb;
};
typedef std::shared_ptr<CTcpServer> TcpServerPtr;


#endif //KYHTTPSERVER_TCPSERVER_H
