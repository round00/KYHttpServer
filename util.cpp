//
// Created by gjk on 12/10/19.
//
#include "util.h"
#include <arpa/inet.h>

std::string addr2str(const struct sockaddr_in& addr){
    char buf[32];
    ::inet_ntop(AF_INET,&addr.sin_addr, buf, sizeof buf);

    std::string s(buf);
    int port = ntohs(addr.sin_port);
    s+=":";
    s += std::to_string(port);
    return s;
}