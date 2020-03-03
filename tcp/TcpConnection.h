//
// Created by gjk on 2020/3/3.
//

#ifndef KYHTTPSERVER_TCPCONNECTION_H
#define KYHTTPSERVER_TCPCONNECTION_H

#include <memory>
#include <functional>
#include "Buffer.h"

typedef std::function<void(BufferPtr)> ReadCallback;
class CTcpConnection{
public:
    int                 getFd(){return  m_fd;}
    void                setReadCallback(const ReadCallback& cb)
    {m_readCallback = cb;}
private:
    int                 m_fd;
    ReadCallback        m_readCallback;
};

typedef std::shared_ptr<CTcpConnection> TcpConnPtr;
#endif //KYHTTPSERVER_TCPCONNECTION_H
