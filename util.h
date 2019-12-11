//
// Created by gjk on 12/10/19.
//

#ifndef KYHTTPSERVER_UTIL_H
#define KYHTTPSERVER_UTIL_H

#include <netinet/in.h>
#include <string>
#include <vector>

const int MAX_BUF_LEN = 1024;
std::string addr2str(const struct sockaddr_in& addr);
std::vector<std::string> spiltstring(const std::string& str, char c);
std::vector<std::string> spiltstring(const std::string& str, const std::string& chr);
#endif //KYHTTPSERVER_UTIL_H
