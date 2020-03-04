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
    //读取一行，返回值大于等于0表示读取的字节数，小于0表示读取失败
    int                 readALine(std::string& line);
    //返回可读的长度
    int                 readAbleLength();
    int                 readNbytes(std::string& buf, int n);
};
typedef std::shared_ptr<CBuffer> BufferPtr;
#endif //KYHTTPSERVER_BUFFER_H
