//
// Created by gjk on 12/10/19.
//

#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include "HttpServer.h"


CHttpServer::CHttpServer()
:m_listenHost("0.0.0.0"), m_listenPort(9876),m_threadCount(2),
m_tcpServer(new CTcpServer())
{

}

bool CHttpServer::init() {
    m_tcpServer->init();
    m_tcpServer->setConnectionCb(std::bind(
            &CHttpServer::onNewConnection, this, std::placeholders::_1));
    return true;
}

void CHttpServer::loop() {

}

void CHttpServer::addController(const string &uri, const Controller &controller) {
    m_controllers[uri] = controller;
}

void CHttpServer::delController(const string& uri) {
    auto it = m_controllers.find(uri);
    if(it == m_controllers.end()){
        return;
    }
    m_controllers.erase(it);
}

Controller CHttpServer::getController(const string &uri) {
    auto it = m_controllers.find(uri);
    if(it == m_controllers.end())
        return nullptr;
    return it->second;
}


void CHttpServer::onNewConnection(const TcpConnPtr &conn) {
    HttpConnPtr httpConn(new CHttpConnection());
    httpConn->setConnState(CONN_CONNECTED);
    m_httpConnections[conn->getFd()] = httpConn;
    conn->setReadCallback(std::bind(&CHttpConnection::onNewRequest,
            httpConn.get(), std::placeholders::_1, std::placeholders::_2));
}