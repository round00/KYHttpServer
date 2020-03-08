//
// Created by gjk on 2020/3/5.
//

#ifndef KYHTTPSERVER_EVENTLOOP_H
#define KYHTTPSERVER_EVENTLOOP_H

#include "Epoller.h"
#include "TcpConnection.h"
#include <memory>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <list>

class CTcpServer;
class CEventLoop{
public:
    CEventLoop(CTcpServer* serverPtr);
    ~CEventLoop();
    bool            start();
    void            quit();
    void            loop();
    bool            isRunning;
    void            dispatchEvent(CEvent* event);
    bool            addNewClient(int fd);

    bool            addEvent(CEvent* event);
    bool            changeEvent(CEvent* event);
    bool            delEvent(CEvent* event);
private:
    void            onNewConnection(CEvent* event);
    void            onRemoveConnection(const TcpConnPtr& conn);
    void            threadFuncion();

private:

    CTcpServer*     m_tcpServer;
    bool            m_bRunning;

//    // pipefd的写端，当需要当前线程执行某项工作时，
//    // 把对应的客户端fd通过这个pipefd传过来，使用这种方式避免了多线程的操作
//    //>0表示新连接fd，-1退出线程
//    int             m_sendTaskFd;
    //接收pipeFd发过来的任务的事件
    CEvent*         m_recvTaskEvent;
    std::mutex      m_mutex;
    std::list<int>  m_newFds;

    CEpoller        m_epoll;
    std::thread     m_thread;
    //保存当前事件循环中持有的所有连接键是fd
    std::unordered_map<int, TcpConnPtr>
                    m_connections;
};
#endif //KYHTTPSERVER_EVENTLOOP_H
