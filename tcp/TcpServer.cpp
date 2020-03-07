//
// Created by gjk on 2020/3/3.
//

#include "TcpServer.h"
#include "Event.h"
#include "SocketUtil.h"
#include "Logger.h"
#include "EventLoop.h"
const int MAX_THREAD_COUNT = 20;

CTcpServer::CTcpServer(int port, int threadCount) :
m_listenPort(port),m_threadCount(threadCount),
m_acceptEvent(nullptr),m_baseLoop(new CEventLoop(this)),m_nextLoop(0){

}
CTcpServer::~CTcpServer() {
    delete m_acceptEvent;
    delete m_baseLoop;
    for(auto ioLoop:m_ioLoops){
        delete ioLoop;
    }
}

bool CTcpServer::init() {
    //默认2个IO线程
    if(m_threadCount<1 ||m_threadCount>MAX_THREAD_COUNT)
        m_threadCount = 2;
    //启动IO事件循环
    for(int i = 0;i<m_threadCount; ++i){
        m_ioLoops.emplace_back(new CEventLoop(this));
        m_ioLoops.back()->start();
    }
    //创建监听的fd
    int serverfd = SocketUtil::openListenedFd(m_listenPort);
    //创建监听事件
    m_acceptEvent = new CEvent(serverfd, EV_TYPE_READ);

    m_acceptEvent->setReadCallback(std::bind(
            &CTcpServer::onAccept, this, std::placeholders::_1));
    m_baseLoop->addEvent(m_acceptEvent);
    return true;
}

void CTcpServer::loop() {
    m_baseLoop->loop();
}

void CTcpServer::onAccept(CEvent* event) {
    string chost;
    int cport;
    //接收客户端连接
    int clientfd = SocketUtil::accept(m_acceptEvent->getFd(), chost, cport);
    if(clientfd < 0){
        LOGE("Accept client connection failed");
        return;
    }
    //选择一个线程去处理新连接
    m_ioLoops[m_nextLoop]->addNewClient(clientfd);
    if(m_nextLoop>=m_threadCount)
        m_nextLoop = 0;
}