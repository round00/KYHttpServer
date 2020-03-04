//
// Created by gjk on 2020/3/3.
//

#ifndef KYHTTPSERVER_TCPCONNECTION_H
#define KYHTTPSERVER_TCPCONNECTION_H

#include <memory>
#include <functional>
#include "Buffer.h"

class CTcpConnection;
typedef std::shared_ptr<CTcpConnection> TcpConnPtr;
typedef std::function<void(TcpConnPtr,BufferPtr)> ReadCallback;
typedef std::function<void(TcpConnPtr)> WriteCompleteCallback;
class CTcpConnection{
public:
    int                 getFd(){return  m_fd;}
    void                setReadCallback(const ReadCallback& cb)
    {m_readCallback = cb;}
    void                setWriteCompleteCb(const WriteCompleteCallback& cb)
    {m_writeCompletecb = cb;}

    void                enableRead(bool enable);
    void                enableWrite(bool enable);

    void                send(const std::string& buf);
    void                close();

private:
    int                 m_fd;
    ReadCallback        m_readCallback;
    WriteCompleteCallback
                        m_writeCompletecb;
};


#endif //KYHTTPSERVER_TCPCONNECTION_H
