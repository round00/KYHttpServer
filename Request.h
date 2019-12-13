//
// Created by gjk on 12/10/19.
//

#ifndef KYHTTPSERVER_REQUEST_H
#define KYHTTPSERVER_REQUEST_H

#include <string>
#include <unordered_map>
#include "HttpState.h"
class Request{
public:
    Request(const std::string& data):m_rawData(data){}
    bool parse();
    std::string getUri(){return m_uri;}
    std::string getValue(std::string& key);
    HTTP_VERSION getHttpVersion()const{return m_httpVersion;}
private:
    std::string m_rawData;

    HTTP_REQUEST_METHOD m_requestMethod;
    HTTP_VERSION m_httpVersion;
    std::string m_uri;
    std::unordered_map<std::string, std::string> m_headers;
    std::string m_body;
};
#endif //KYHTTPSERVER_REQUEST_H
