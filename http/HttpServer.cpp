//
// Created by gjk on 12/10/19.
//

#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include "HttpServer.h"

CHttpServer::CHttpServer(int port, int threadCount):
m_tcpServer(new CTcpServer(port, threadCount)),
m_allowMethod(DEFAULT_METHOD)
{
}

bool CHttpServer::init() {
    if(!m_tcpServer->init()){
        return false;
    }
    m_tcpServer->setConnectionCb(std::bind(
            &CHttpServer::onNewConnection, this, std::placeholders::_1));
    m_tcpServer->setDisConnectCb(std::bind(
            &CHttpServer::onDisConnected, this, std::placeholders::_1));
    return true;
}

void CHttpServer::loop() {
    m_tcpServer->loop();
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
    HttpConnPtr httpConn(new CHttpConnection(this));
    httpConn->setConnState(CONN_CONNECTED);
    httpConn->setTcpConn(conn);
    conn->setReadCallback(std::bind(&CHttpConnection::onNewRequest,
            httpConn.get(), std::placeholders::_1, std::placeholders::_2));

    CMutexLock lock(m_connsMutex);
    m_httpConnections[conn->getFd()] = httpConn;
}

void CHttpServer::onDisConnected(const TcpConnPtr &conn) {
    CMutexLock lock(m_connsMutex);
    auto it = m_httpConnections.find(conn->getFd());
    if(it != m_httpConnections.end())
        m_httpConnections.erase(it);
}