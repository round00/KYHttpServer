//
// Created by gjk on 2020/3/3.
//

#ifndef KYHTTPSERVER_HTTPCONNECTION_H
#define KYHTTPSERVER_HTTPCONNECTION_H

#include <memory>
#include "Buffer.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "HttpStates.h"
#include "TcpConnection.h"
#include "HttpServer.h"

class CHttpConnection{
public:
    CHttpConnection();
    void                setConnState(CONNECTION_STATE state)
    {m_connState = state;}
    void                onNewRequest(TcpConnPtr tcpConn, BufferPtr buffer);
    void                onResponseDone(TcpConnPtr tcpConn);
private:
    void                readFirstLine(BufferPtr buffer);
    void                readHeaders(BufferPtr buffer);
    void                readBody(BufferPtr buffer);
    //根据长度读body
    void                readBodyByLength(BufferPtr buffer);
    //根据分块读body
    void                readBodyByChunk(BufferPtr buffer);

    //处理读取请求失败的情况
    void                requestFailed(REQUEST_FAILED reason);
    //处理读取请求成功
    void                requestSucess();
    //分发请求
    void                requestDispath();
    Controller          getController(const string& uri);
    //把响应发给客户端
    void                responseClient();
    //根据http响应构造响应数据包
    string              makeResponsePacket();

    void                resetHttpConnection();

    //当前连接所在的httpserver
    std::weak_ptr<CHttpServer>
                        m_httpServer;
    //当前http连接所在的tcp连接对象
    std::weak_ptr<CTcpConnection>
                        m_tcpConn;

    HttpRequestPtr      m_request;
    HttpResponsePtr     m_response;
    CONNECTION_STATE    m_connState;
};
typedef std::shared_ptr<CHttpConnection> HttpConnPtr;
#endif //KYHTTPSERVER_HTTPCONNECTION_H
