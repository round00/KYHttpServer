//
// Created by gjk on 2019/12/31.
//

#ifndef KYIMSERVER_CONDITION_H
#define KYIMSERVER_CONDITION_H

#include "noncopyable.h"
#include <pthread.h>
/*
 * 条件变量是用来等待而不是上锁的
 * 条件变量用来自动阻塞一个线程，知道特殊情况发生为止
 */
class CMutex;
class CCondition : public noncopyable
{
public:
    CCondition();
    ~CCondition();

    //等待条件发生
    void wait(CMutex& mutex);
    //通知条件发生
    void notify();
    //通知所有该条件变量
    void notifyAll();
private:
    pthread_cond_t  m_condition;
};


#endif //KYIMSERVER_CONDITION_H
