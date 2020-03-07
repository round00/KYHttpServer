//
// Created by gjk on 2020/3/3.
//

#include "HttpMessage.h"

CHttpMessage::CHttpMessage()
:m_httpVersionMajor(1), m_httpVersionMinor(1),
 m_bodyType(BODY_TYPE_UNSURE),m_bodyLen(0),m_bodyChunkSize(-1)
{

}

uint8_t CHttpMessage::getHttpVersion() {
    return m_httpVersionMajor*10 + m_httpVersionMinor;
}

void CHttpMessage::addHeader(const string &key, const string &val){
    m_headers[key] = val;
}

string CHttpMessage::getHeader(const string &key) {
    auto it = m_headers.find(key);
    if(it == m_headers.end())
        return "";
    return it->second;
}

void CHttpMessage::delHeader(const string &key) {
    auto p = m_headers.find(key);
    if(p != m_headers.end()){
        m_headers.erase(p);
    }
}

std::vector<std::pair<string,string>> CHttpMessage::getAllHeaders() {
    std::vector<std::pair<string,string>> headers;
    for(auto p:m_headers){
        headers.emplace_back(p);
    }
    return headers;
}

