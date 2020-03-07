//
// Created by gjk on 2020/3/5.
//
#include <thread>
#include <vector>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include <string.h>
#include "EventLoop.h"
#include "Event.h"
#include "Logger.h"
#include "TcpServer.h"
#include "SocketUtil.h"

CEventLoop::CEventLoop(CTcpServer* serverPtr):
m_tcpServer(serverPtr), m_bRunning(false),m_recvTaskEvent(nullptr){

}

CEventLoop::~CEventLoop() {
    if(m_recvTaskEvent){
        ::close(m_sendTaskFd);
        ::close(m_recvTaskEvent->getFd());
        delete m_recvTaskEvent;
    }
}

bool CEventLoop::start() {
    int pipefd[2];
    if(::pipe(pipefd) < 0){
        LOGE("Create pipe fd failed");
        return false;
    }
    //1是写端
    m_sendTaskFd = pipefd[1];
    //0是读端,设置为非阻塞
    SocketUtil::setNonblock(pipefd[0]);
    m_recvTaskEvent = new CEvent(pipefd[0], EV_TYPE_READ);
    m_recvTaskEvent->setReadCallback(std::bind(
            &CEventLoop::onNewConnection, this, std::placeholders::_1));
    addEvent(m_recvTaskEvent);
    m_thread = std::thread(std::bind(&CEventLoop::threadFuncion, this));
    return true;
}

void CEventLoop::quit() {
    int quitFd = -1;
    ::write(m_sendTaskFd, &quitFd, sizeof(quitFd));
}

void CEventLoop::loop() {
    m_bRunning = true;
    while(true){
        if(!m_bRunning){
            break;
        }
        std::vector<CEvent*> activeEvents;
        int nRet = m_epoll.poll(activeEvents);

        for(int i = 0;i<nRet; ++i) {
            dispatchEvent(activeEvents[i]);
        }
    }
}

void CEventLoop::dispatchEvent(CEvent *event) {
    if(event->getEvents() & (EPOLLIN | EPOLLPRI)){
        if(event->getReadCallback()){
            event->getReadCallback()(event);
        }
    }
    else if(event->getEvents() & (EPOLLOUT)){
        if(event->getWriteCallback()){
            event->getWriteCallback()(event);
        }
    }
}

bool CEventLoop::addNewClient(int fd) {
    LOGI("Eventloop %d, Accept connection fd=%d", m_sendTaskFd, fd);
    if(::write(m_sendTaskFd, &fd, sizeof(fd)) < 0){
        std::string err = strerror(errno);
        LOGE("addNewConnection failed");
        return false;
    }
    return true;
}

bool CEventLoop::addEvent(CEvent *event) {
    return m_epoll.addEvent(event);
}

bool CEventLoop::changeEvent(CEvent *event) {
    return m_epoll.modifyEvent(event);
}

bool CEventLoop::delEvent(CEvent *event) {
    return m_epoll.delEvent(event);
}

void CEventLoop::onNewConnection(CEvent *event) {
    int clientfd;
    ::read(event->getFd(), &clientfd, sizeof clientfd);
    if(clientfd==-1){
        //-1表示退出
        m_bRunning = false;
        return;
    }

    //创建新的连接
    TcpConnPtr conn(new CTcpConnection(this, clientfd));
    conn->setCloseCallback(std::bind(
            &CEventLoop::onRemoveConnection, this, std::placeholders::_1));

    m_connections[clientfd] = conn;
    if(m_tcpServer->getConnectionCb()){
        m_tcpServer->getConnectionCb()(conn);
    }
    LOGI("EventLoop %d create new tcp connection, fd=%d",event->getFd(), clientfd);
}

void CEventLoop::onRemoveConnection(const TcpConnPtr &conn) {
    //断连回调
    if(m_tcpServer->getDisConnectionCb()){
        m_tcpServer->getDisConnectionCb()(conn);
    }

    LOGI("Connection fd=%d remove", conn->getFd());
    //删掉保存的conn对象，使得conn能够析构
    m_connections.erase(conn->getFd());

}

void CEventLoop::threadFuncion() {
    loop();
}

