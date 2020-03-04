//
// Created by gjk on 2020/3/3.
// Http请求类
//

#ifndef KYHTTPSERVER_HTTPREQUEST_H
#define KYHTTPSERVER_HTTPREQUEST_H

#include <memory>
#include "HttpMessage.h"
#include "HttpStates.h"

class CHttpRequest : public CHttpMessage{
public:
    CHttpRequest();
    void                setUri(const string& uri){m_uri = uri;}
    string              getUri(){return m_uri;}

    void                setMethod(REQUEST_METHOD method){m_method = method;}
    REQUEST_METHOD      getMethod(){return m_method;}

private:
    string              m_uri;
    REQUEST_METHOD      m_method;

};
typedef std::shared_ptr<CHttpRequest> HttpRequestPtr;

#endif //KYHTTPSERVER_HTTPREQUEST_H
