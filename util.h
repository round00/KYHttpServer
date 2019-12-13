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
//"GET /hello HTTP/1.1\r\nHost: localhost:9982\r\nConnection: keep-alive\r\nCache-Control: max-age=0\r\nUpgrade-Insecure-Requests: 1\r\nUser-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/78.0.3904.108 Safari/537.36\r\nSec-Fetch-User: ?1\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3\r\nSec-Fetch-Site: none\r\nSec-Fetch-Mode: navigate\r\nAccept-Encoding: gzip, deflate, br\r\nAccept-Language: en-US,en;q=0.9\r\n\r\n"