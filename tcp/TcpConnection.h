//
// Created by gjk on 2020/3/3.
//

#ifndef KYHTTPSERVER_TCPCONNECTION_H
#define KYHTTPSERVER_TCPCONNECTION_H

#include <memory>
#include <functional>
#include "Buffer.h"

class CEvent;
class CEventLoop;
class CTcpConnection;

typedef std::shared_ptr<CTcpConnection> TcpConnPtr;
typedef std::function<void(const TcpConnPtr&,BufferPtr)> ReadCallback;
typedef std::function<void(const TcpConnPtr&)> WriteCompleteCallback;
typedef std::function<void(const TcpConnPtr&)> CloseCallback;

class CTcpConnection:public std::enable_shared_from_this<CTcpConnection>{
public:
    CTcpConnection(CEventLoop* loop, int fd);
    ~CTcpConnection();
    int                 getFd();
//    CEvent*             getEvent(){return m_event;}
    void                setReadCallback(const ReadCallback& cb)
    {m_readCallback = cb;}
    void                setWriteCompleteCb(const WriteCompleteCallback& cb)
    {m_writeCompletecb = cb;}
    void                setCloseCallback(const CloseCallback& cb)
    {m_closeCb = cb;}


    void                enableRead(bool enable);
    void                enableWrite(bool enable);
    void                disableAll();

    void                send(const std::string& buf);
    void                closeConnection();

    BufferPtr           getInputBuffer(){return m_inputBuffer;}
    BufferPtr           getOutputBuffer(){return m_outputBuffer;}

private:
    void                onReadEvent(CEvent* event);
    void                onWriteEvent(CEvent* event);

private:
    ReadCallback        m_readCallback;
    WriteCompleteCallback
                        m_writeCompletecb;
    CloseCallback       m_closeCb;

    BufferPtr           m_inputBuffer;
    BufferPtr           m_outputBuffer;
    //不能delete这个
    CEventLoop*         m_ownerLoop;
    CEvent*             m_event;
};


#endif //KYHTTPSERVER_TCPCONNECTION_H
