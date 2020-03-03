//
// Created by gjk on 2019/12/31.
//

#include "Condition.h"
#include "util.h"
#include "Mutex.h"

CCondition::CCondition()
{
    if(::pthread_cond_init(&m_condition, nullptr) < 0){
        crash();
    }
}

CCondition::~CCondition()
{
    ::pthread_cond_destroy(&m_condition);
}

void CCondition::wait(CMutex& mutex)
{
    ::pthread_cond_wait(&m_condition, &mutex.m_mutex);
}

void CCondition::notify()
{
    ::pthread_cond_signal(&m_condition);
}

void CCondition::notifyAll()
{
    ::pthread_cond_broadcast(&m_condition);
}