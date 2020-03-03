//
// Created by gjk on 2019/12/31.
//

#ifndef KYIMSERVER_THREAD_H
#define KYIMSERVER_THREAD_H

#include "noncopyable.h"
#include <pthread.h>

class CThread : public noncopyable
{
public:
    typedef void*(*ThreadFunc)(void*);
    explicit CThread(ThreadFunc func);
    ~CThread();

public:
    bool start();
    bool isRunning();
    void stop();

private:
    bool        m_bInit;
    ThreadFunc  m_function;
    pthread_t   m_thread;
};

#endif //KYIMSERVER_THREAD_H
