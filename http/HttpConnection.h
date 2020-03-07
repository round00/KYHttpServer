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


class CHttpServer;
typedef std::function<void(const HttpRequestPtr&, HttpResponsePtr)> Controller;

class CHttpConnection{
public:
    CHttpConnection(CHttpServer* httpServer);
    void                setConnState(CONNECTION_STATE state)
    {m_connState = state;}
    void                setTcpConn(TcpConnPtr conn){m_tcpConn = conn;}


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
    bool                requestDispath();
    Controller          getController(const string& uri);
    //把响应发给客户端
    void                responseClient();
    void                responseClientError(int code, const string& reason);
    //根据http响应m_response构造响应数据包
    string              makeResponsePacket();
    //重置http连接，将它恢复到刚创建连接时的状态
    void                resetHttpConnection();
    //将header组装好加入到末尾
    void                header2Str(string &str, const string& key, const string& val);
    bool                connNeedClose();
    //当前连接所在的httpserver
    CHttpServer*        m_httpServer;
    //当前http连接所在的tcp连接对象
    std::weak_ptr<CTcpConnection>
                        m_tcpConn;

    HttpRequestPtr      m_request;
    HttpResponsePtr     m_response;
    CONNECTION_STATE    m_connState;
};
typedef std::shared_ptr<CHttpConnection> HttpConnPtr;
#endif //KYHTTPSERVER_HTTPCONNECTION_H
