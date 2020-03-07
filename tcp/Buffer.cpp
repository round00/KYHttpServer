//
// Created by gjk on 2020/3/3.
//

#include "Buffer.h"
#include <unistd.h>
#include <string.h>

const int INIT_BUFFER_SIZE = 64;
const char* CRLF = "\r\n";

CBuffer::CBuffer():
m_begin(0),m_end(0){
    m_buffer.reserve(INIT_BUFFER_SIZE);
}

int CBuffer::readALine(std::string &line) {
    int len = readableLength();
    if(len < 2){
        return -1;
    }
    auto pos = m_buffer.find(CRLF, m_begin, strlen(CRLF));
    if(pos == string::npos){
        return -1;
    }

    line = m_buffer.substr(m_begin, pos-m_begin);
    //跳过CRLF
    m_begin = pos + 2;
    adjust();
    return line.length();
}

int CBuffer::readableLength() {
    return m_end-m_begin;
}

int CBuffer::readNbytes(std::string &buf, int n) {
    int toread = n;
    if(n>readableLength())
        toread = readableLength();
    buf = m_buffer.substr(m_begin, toread);
    m_begin += toread;
    adjust();
    return toread;
}

int CBuffer::writeToFd(int fd) {
    int nleft = readableLength(), nall = nleft;
    if(nleft<=0)
        return 0;
    char* p = &m_buffer[m_begin];
    while(nleft>0){
        int n = ::write(fd, p, nleft);
        if(n>0){    //写入了n个字节
            nleft -= n;
            p += n;
        }else if(n<0){  //出错
            if(errno==EINTR){   //被打断，可恢复
                continue;
            }else{  //不可恢复错误
                if(nall > nleft)break;  //写入了一点数据，正常返回
                else return -1; //一点没写入直接返回
            }
        }
    }

    m_begin += nall - nleft;
    adjust();
    return nall-nleft;
}


void CBuffer::drainAll() {
    m_begin = 0;
    m_end = 0;
}

int CBuffer::writeableLength() {
    return m_buffer.size()-m_begin;
}

int CBuffer::readFromFd(int fd) {
    char buf[4096];
    int nread = 0;
    while(true){
        int n = ::read(fd, buf, sizeof buf);
        if(n > 0){  //正确读出了数据
            appendBytes(buf, n);
            nread += n;
        }else if(n==0){ //n==0表明读完了
            break;
        }else{
            if(errno == EAGAIN){    //表示没有数据了
                break;
            }else if(errno == EINTR){   //被打断
                continue;
            }else{  //出现错误了
                return -1;
            }
        }
    }
    return nread;
}

int CBuffer::appendBytes(const string &buf) {
    int n = buf.length();
    makeSpace(n);
    m_buffer.replace(m_end, n, buf);
    m_end += n;
    return n;
}

int CBuffer::appendBytes(const char *buf, int len) {
    makeSpace(len);
    m_buffer.replace(m_end, len, buf);
    m_end += len;
    return len;
}

void CBuffer::makeSpace(int n) {
    if(writeableLength()>=n){
        return;
    }
    int cursize = m_buffer.size();
    //空间不够时，每次扩大所缺空间的2倍
    cursize += (n-writeableLength())*2;
    m_buffer.reserve(cursize);
}

void CBuffer::adjust() {
    if(readableLength()==0){
        m_begin = 0;
        m_end = 0;
    }
}