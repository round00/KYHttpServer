//
// Created by gjk on 2020/3/6.
//

#ifndef KYHTTPSERVER_SOCKETUTIL_H
#define KYHTTPSERVER_SOCKETUTIL_H

#include <string>
using std::string;

namespace SocketUtil{
    int         openListenedFd(int port);
    int         accept(int fd, string& host, int& port);
    int         setNonblock(int fd);
    int         setReusePort(int fd);
}

#endif //KYHTTPSERVER_SOCKETUTIL_H
