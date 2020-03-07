//
// Created by gjk on 2020/3/3.
//

#include "TcpConnection.h"
#include "Event.h"
#include "EventLoop.h"
#include <unistd.h>

CTcpConnection::CTcpConnection(CEventLoop *loop, int fd):
m_inputBuffer(new CBuffer()), m_outputBuffer(new CBuffer()),
m_ownerLoop(loop),m_event(new CEvent(fd, EV_TYPE_READ))
{
    m_event->setReadCallback(std::bind(
            &CTcpConnection::onReadEvent, this, std::placeholders::_1));
    m_event->setWriteCallback(std::bind(
            &CTcpConnection::onWriteEvent, this, std::placeholders::_1));
    m_ownerLoop->addEvent(m_event);
}

CTcpConnection::~CTcpConnection() {
    ::close(m_event->getFd());
    delete m_event;

}

int CTcpConnection::getFd() {
    return m_event->getFd();
}

void CTcpConnection::enableRead(bool enable) {
    int event = m_event->getEvents();
    if(enable){
        event |= EV_TYPE_READ;
    }else{
        event &= (~EV_TYPE_READ);
    }

    if(event != m_event->getEvents()){
        m_event->setEvents(event);
        m_ownerLoop->changeEvent(m_event);
    }
}

void CTcpConnection::enableWrite(bool enable){
    int event = m_event->getEvents();
    if(enable){
        event |= EV_TYPE_WRITE;
    }else{
        event &= (~EV_TYPE_WRITE);
    }
    if(event != m_event->getEvents()){
        m_event->setEvents(event);
        m_ownerLoop->changeEvent(m_event);
    }
}

void CTcpConnection::disableAll() {
    m_ownerLoop->delEvent(m_event);
}

void CTcpConnection::send(const std::string &buf) {
    //把数据放到输出buffer中，打开写事件的监听，只要能写就会回调回来
    m_outputBuffer->appendBytes(buf);
    enableWrite(true);
}

void CTcpConnection::onReadEvent(CEvent *event) {
    int fd = event->getFd();
    int nRead = m_inputBuffer->readFromFd(fd);
    if(nRead > 0){
        if(m_readCallback){
            m_readCallback(shared_from_this(), m_inputBuffer);
        }
    }else {
        closeConnection();
    }
}

void CTcpConnection::onWriteEvent(CEvent *event) {
    int fd = event->getFd();
    //有数据要写
    if(m_outputBuffer->readableLength()>0){
        int nWrite = m_outputBuffer->writeToFd(fd);
        if(nWrite > 0){
            if(m_outputBuffer->readableLength() == 0){
                m_writeCompletecb(shared_from_this());
            }
        }else{
            closeConnection();
        }
    }
}

void CTcpConnection::closeConnection() {
    //取消监听的所有事件
    disableAll();
    //断开回调
    m_closeCb(shared_from_this());
    //把缓冲区的内容都删掉
    m_inputBuffer->drainAll();
    m_outputBuffer->drainAll();
}