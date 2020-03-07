//
// Created by gjk on 2020/3/3.
//

#ifndef KYHTTPSERVER_BUFFER_H
#define KYHTTPSERVER_BUFFER_H

#include <memory>
#include <string>
using std::string;

class CBuffer{
public:
    CBuffer();
    //================从buffer中读取数据================
    //读取一行，返回值大于等于0表示读取的字节数，小于0表示读取失败
    int                 readALine(string& line);
    //返回可读的长度
    int                 readableLength();
    //最多读取n个字节，成功返回读取的字节数，失败返回-1
    int                 readNbytes(string& buf, int n);
    int                 writeToFd(int fd);
    //将所有数据删除掉
    void                drainAll();
    //================往buffer中添加数据================
    int                 writeableLength();
    int                 readFromFd(int fd);
    //添加数据到buffer的最后，返回添加的字节数
    int                 appendBytes(const string& buf);
    int                 appendBytes(const char* buf, int len);

private:
    //增大空间，使得能够盛下n个字节
    void                makeSpace(int n);
    //调整首尾指针，增大空间利用率。如果当前可读字节数为0，则把双指针都移到最前面
    void                adjust();
    //用一个string来保存数据
    string              m_buffer;
    //有效数据的开始
    int                 m_begin;
    //有效数据的结尾+1
    int                 m_end;
};
typedef std::shared_ptr<CBuffer> BufferPtr;
#endif //KYHTTPSERVER_BUFFER_H
