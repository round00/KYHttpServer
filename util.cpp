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

std::vector<std::string> spiltstring(const std::string& str, char c){
    std::vector<std::string> vecStr;
    int beginPos = 0;
    for(int i = 0; i<str.size(); ++i){
        if(str[i] == c){
            vecStr.push_back(str.substr(beginPos, i - beginPos));
            beginPos = i+1;
        }
    }
    if(beginPos == 0){
        vecStr.push_back(str);
    }
    return vecStr;
}

//
std::vector<std::string> spiltstring(const std::string& str, const std::string& chr){

}