//
// Created by gjk on 2019/12/31.
//

#include "Thread.h"
#include <signal.h>


CThread::CThread(ThreadFunc func)
    :m_bInit(false),m_function(func),m_thread(0)
{
}
CThread::~CThread()
{
    stop();
}

bool CThread::start()
{
    if(!m_function) //线程函数都还没初始化
    {
        return false;
    }

    pthread_attr_t attr;
    if(::pthread_attr_init(&attr))
    {
        return false;
    }
    if(::pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED))
    {
        return false;
    }
    if(::pthread_create(&m_thread, &attr, m_function, nullptr)){
        return false;
    }
    m_bInit = true;
    return true;
}

bool CThread::isRunning()
{
    if(!m_bInit){
        return false;
    }
    return ::pthread_kill(m_thread, 0) == 0;
}

void CThread::stop()
{
    if(isRunning()){
        ::pthread_cancel(m_thread);
    }
}