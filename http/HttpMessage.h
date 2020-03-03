//
// Created by gjk on 2020/3/3.
// Http报文类
//

#ifndef KYHTTPSERVER_HTTPMESSAGE_H
#define KYHTTPSERVER_HTTPMESSAGE_H

#include <string>
#include <unordered_map>
#include <stdint.h>
using std::string;

class CHttpMessage{
public:
    CHttpMessage();

    void                addHeader(const string& key, const string& val);
    void                delHeader(const string& key);
private:
    //Http版本号
    uint8_t             m_httpVersionMajor;
    uint8_t             m_httpVersionMinor;

    //报文头
    std::unordered_map<string, string>
                        m_headers;
    //报文主体
    string              m_body;

};

#endif //KYHTTPSERVER_HTTPMESSAGE_H
