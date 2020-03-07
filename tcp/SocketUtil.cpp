//
// Created by gjk on 2020/3/6.
//

#include "SocketUtil.h"
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>

namespace SocketUtil{
    int openListenedFd(int port){
        int fd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(fd<0){
            perror("socket create failed");
            return -1;
        }
        setNonblock(fd);
        setReusePort(fd);

        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(struct sockaddr_in));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = INADDR_ANY;

        if(::bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0){
            perror("socket bind failed");
            return -1;
        }
        if(::listen(fd, SOMAXCONN)<0){
            perror("socket listen failed");
            return -1;
        }
        return fd;
    }

    int accept(int fd, string& host, int& port){
        struct sockaddr_in addr;
        socklen_t socklen = sizeof(addr);
        int clientfd = ::accept4(fd, (struct sockaddr*)&addr, &socklen, SOCK_NONBLOCK | SOCK_CLOEXEC);
        if(clientfd<0){
            perror("socket accept failed");
            return -1;
        }
        port = ntohs(addr.sin_port);
        char buf[32];
        ::inet_ntop(AF_INET, &addr.sin_addr, buf, sizeof(buf));
        host = buf;
        return clientfd;
    }

    int setNonblock(int fd){
        int f = fcntl(fd, F_GETFL, 0);
        return fcntl(fd, F_SETFL, f|O_NONBLOCK);
    }

    int setReusePort(int fd){
        int op = 1;
        return ::setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &op, sizeof(op));
    }

}
