//
// Created by gjk on 2020/3/3.
// Http响应类
//

#ifndef KYHTTPSERVER_HTTPRESPONSE_H
#define KYHTTPSERVER_HTTPRESPONSE_H

#include <memory>
#include "HttpMessage.h"

class CHttpResponse : public CHttpMessage{
public:
    CHttpResponse();
    void                setResponseLine(uint16_t code, const string& reason)
    {m_responseCode = code; m_responseReason = reason;}
    uint16_t            getResponseCode(){return m_responseCode;}
    string              getResponseReason(){return m_responseReason;}

private:
    //响应码
    uint16_t            m_responseCode;
    //响应短消息
    string              m_responseReason;
};
typedef std::shared_ptr<CHttpResponse> HttpResponsePtr;

#endif //KYHTTPSERVER_HTTPRESPONSE_H
