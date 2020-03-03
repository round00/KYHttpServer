//
// Created by gjk on 2020/3/3.
//

#ifndef KYHTTPSERVER_HTTPCONNECTION_H
#define KYHTTPSERVER_HTTPCONNECTION_H

#include <memory>
#include "Buffer.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

class CHttpConnection{
public:
    CHttpConnection();
    void                onNewRequest(BufferPtr buffer);
private:
    HttpRequestPtr      m_request;
    HttpResponsePtr     m_response;
};
typedef std::shared_ptr<CHttpConnection> HttpConnPtr;
#endif //KYHTTPSERVER_HTTPCONNECTION_H
