//
// Created by gjk on 12/10/19.
//

#ifndef KYHTTPSERVER_HTTPSERVER_H
#define KYHTTPSERVER_HTTPSERVER_H

#include <string>
#include <map>
#include <unordered_map>
#include <functional>
#include "Mutex.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "HttpConnection.h"
#include "TcpServer.h"
using std::string;

//取Controller这个名字，是模仿java处理web请求
typedef std::function<void(const HttpRequestPtr&, HttpResponsePtr)> Controller;
class CHttpServer{
public:
    CHttpServer(int port = 9876, int threadCount = 2);
    bool            init();
    void            loop();

    void            setAllowMethod(uint16_t methods){m_allowMethod = methods;}
    uint16_t        getAllowMethod(){return m_allowMethod;}

    void            addController(const string& uri, const Controller& controller);
    void            delController(const string& uri);
    Controller      getController(const string& uri);

    void            setDefaultController(const Controller& controller)
    {m_defaultController = controller;}
    Controller      getDefaultController(){ return m_defaultController;}


private:
    void            onNewConnection(const TcpConnPtr& conn);
    void            onDisConnected(const TcpConnPtr& conn);
    //用户设置的请求回调
    std::unordered_map<string, Controller>
                    m_controllers;
    //默认的请求回调，即在m_controllers中找不到对应的uri时，就调用这个
    Controller      m_defaultController;

    //用于保护m_httpConnections的锁
    CMutex          m_connsMutex;

    //当前的http连接，以对方的fd唯一标识，
    //忽略Pipelining，HTTP/1.1中一个tcp连接同时只能有一个HTTP连接
    std::unordered_map<int, HttpConnPtr>
                    m_httpConnections;
    TcpServerPtr    m_tcpServer;
    uint16_t        m_allowMethod;

};
#endif //KYHTTPSERVER_HTTPSERVER_H
