//
// Created by gjk on 2020/3/3.
//

#include "HttpMessage.h"

CHttpMessage::CHttpMessage()
:m_httpVersionMajor(1), m_httpVersionMinor(1)
{

}

void CHttpMessage::addHeader(const string &key, const string &val){
    m_headers[key] = val;
}

void CHttpMessage::delHeader(const string &key) {
    auto p = m_headers.find(key);
    if(p != m_headers.end()){
        m_headers.erase(p);
    }
}