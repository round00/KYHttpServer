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
private:
    uint16_t            m_responseCode;
    string              m_responseReason;
};
typedef std::shared_ptr<CHttpResponse> HttpResponsePtr;

#endif //KYHTTPSERVER_HTTPRESPONSE_H
