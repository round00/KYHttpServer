//
// Created by gjk on 2020/3/5.
//

#include <sys/epoll.h>
#include <string.h>
#include "Epoller.h"
#include "Logger.h"
#include "Event.h"


CEpoller::CEpoller():
m_eventsContainer(8){
    m_epollid = ::epoll_create1(EPOLL_CLOEXEC);
    if(m_epollid < 0){
        LOGF("epoll fd create failed: err=%s", strerror(errno));
        return;
    }

}

bool CEpoller::addEvent(CEvent *event) {
    return epollUpdate(EPOLL_CTL_ADD, event);
}

bool CEpoller::modifyEvent(CEvent *event) {
    return epollUpdate(EPOLL_CTL_MOD, event);
}

bool CEpoller::delEvent(CEvent *event) {
    return epollUpdate(EPOLL_CTL_DEL, event);
}

int CEpoller::poll(std::vector<CEvent*> &activeEvents) {
//    m_eventsContainer.clear();
    int nRet = ::epoll_wait(m_epollid, m_eventsContainer.data(),
            m_eventsContainer.size(), -1);
    if(nRet <= 0){
        std::string err = strerror(errno);
        return nRet;
    }
    for(int i = 0;i<nRet; ++i){
        CEvent* e = (CEvent*)m_eventsContainer[i].data.ptr;
        activeEvents.push_back(e);
    }
    //如果当前container不足以盛下epoll返回的事件了，则给它扩容
    if(nRet == m_eventsContainer.size()){
        m_eventsContainer.resize(nRet*2);
    }

    return nRet;
}

bool CEpoller::epollUpdate(int op, CEvent *event) {
    if(!event)
        return false;
    struct epoll_event edata;
    memset(&edata, 0, sizeof(edata));
    edata.events = event->getEvents();
    edata.data.ptr = event;
    if(::epoll_ctl(m_epollid, op, event->getFd(), &edata) < 0){
        LOGE("epoll_ctl failed");
        return false;
    }
    return true;
}