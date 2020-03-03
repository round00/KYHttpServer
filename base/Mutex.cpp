//
// Created by gjk on 2019/12/31.
//

#include "Mutex.h"
#include "util.h"

CMutex::CMutex()
{
    if(::pthread_mutex_init(&m_mutex, nullptr)<0){
        //互斥锁初始化失败，挂掉程序，以免后续对数据造成威胁
        crash();
    }
}
CMutex::~CMutex()
{
    ::pthread_mutex_destroy(&m_mutex);
}


CMutexLock::CMutexLock(CMutex &mutex)
    :m_mutex(mutex)
{
    ::pthread_mutex_lock(&m_mutex.m_mutex);
}

CMutexLock::~CMutexLock()
{
    ::pthread_mutex_unlock(&m_mutex.m_mutex);
}