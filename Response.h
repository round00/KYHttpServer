//
// Created by gjk on 12/10/19.
//

#ifndef KYHTTPSERVER_RESPONSE_H
#define KYHTTPSERVER_RESPONSE_H

#include <string>
#include <unordered_map>
#include "HttpState.h"
class Response{
public:
    Response(HTTP_VERSION version, int status, const std::string& msg);
    std::string toString();
    void addHeaders(const std::string& key, const std::string& value);
    void setBody(const std::string& body);
private:
    int m_statusCode;
    HTTP_VERSION m_httpVersion;
    std::string m_msg;
    std::unordered_map<std::string, std::string> m_headers;
    std::string m_body;
};
#endif //KYHTTPSERVER_RESPONSE_H
