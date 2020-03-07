//
// Created by gjk on 2020/3/5.
//

#ifndef KYHTTPSERVER_EPOLLER_H
#define KYHTTPSERVER_EPOLLER_H

#include <vector>

struct epoll_event;
class CEvent;
class CEpoller{
public:
    CEpoller();

    bool            addEvent(CEvent* event);
    bool            modifyEvent(CEvent* event);
    bool            delEvent(CEvent* event);
    int             poll(std::vector<CEvent*>& activeEvents);
private:
    bool            epollUpdate(int op, CEvent* event);
private:
    int             m_epollid;
    //这个是用来存放从epoll_wait中返回的事件的
    std::vector<struct epoll_event>
                    m_eventsContainer;
};
#endif //KYHTTPSERVER_EPOLLER_H
