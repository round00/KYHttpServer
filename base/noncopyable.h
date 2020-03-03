//
// Created by gjk on 2019/12/31.
//

#ifndef KYIMSERVER_NONCOPYABLE_H
#define KYIMSERVER_NONCOPYABLE_H
class noncopyable
{
public:
    noncopyable()= default;
    ~noncopyable() = default;

    noncopyable(const noncopyable&)= delete;
    noncopyable& operator=(const noncopyable&)= delete;
};
#endif //KYIMSERVER_NONCOPYABLE_H
