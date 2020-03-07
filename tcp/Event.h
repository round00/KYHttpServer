//
// Created by gjk on 2020/3/5.
//

#ifndef KYHTTPSERVER_EVENT_H
#define KYHTTPSERVER_EVENT_H

#include <memory>
#include <functional>
#include <sys/epoll.h>

enum EV_TYPE{
    EV_TYPE_READ        = EPOLLIN | EPOLLPRI|EPOLLET,
    EV_TYPE_WRITE       = EPOLLOUT ,
};

class CEvent;
typedef std::function<void(CEvent*)> EventCallBack;
class CEvent{
public:
    CEvent(int fd, uint32_t events):m_fd(fd), m_events(events){}
    int                 getFd(){return m_fd;}

    void                setEvents(uint16_t event){m_events = event;}
    uint16_t            getEvents(){return m_events;}

    void                setReadCallback(const EventCallBack& cb)
    {m_readCallback = cb;}
    EventCallBack       getReadCallback(){return m_readCallback;}

    void                setWriteCallback(const EventCallBack& cb)
    {m_writeCallback = cb;}
    EventCallBack       getWriteCallback(){return m_writeCallback;}

private:
    int                 m_fd;
    uint32_t            m_events;
    EventCallBack       m_readCallback;
    EventCallBack       m_writeCallback;
};
typedef std::shared_ptr<CEvent> EventPtr;

#endif //KYHTTPSERVER_EVENT_H
