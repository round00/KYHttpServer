//
// Created by gjk on 2019/12/31.
//

#ifndef KYIMSERVER_MUTEX_H
#define KYIMSERVER_MUTEX_H

#include "noncopyable.h"
#include <pthread.h>

class CMutex : public noncopyable
{
public:
    CMutex();
    ~CMutex();
    friend class CMutexLock;
    friend class CCondition;
private:
    pthread_mutex_t m_mutex;
};

class CMutexLock : public noncopyable
{
public:
    explicit CMutexLock(CMutex& mutex);
    ~CMutexLock();
private:
    CMutex&     m_mutex;
};

#endif //KYIMSERVER_TESTMUTEX_H
