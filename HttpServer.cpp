//
// Created by gjk on 12/10/19.
//

#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include "HttpServer.h"
#include "Logger.h"
#include "util.h"
#include "Request.h"
#include "Response.h"

CHttpServer::CHttpServer(int port)
    :m_port(port),
    m_bRunning(false)
{
}

void CHttpServer::start() {
    //create socket
    int serverfd = ::socket(AF_INET, SOCK_STREAM, 0);
    if(serverfd<0){
        Logger::ERROR("create socket failed, err=%s\n", strerror(errno));
        return;
    }
    //bind socket
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(m_port);
    int ret = ::bind(serverfd, (sockaddr*)&addr, sizeof addr);
    if(ret < 0){
        Logger::ERROR("bind socket failed, err=%s\n", strerror(errno));
        return;
    }
    //listen socket
    ret = ::listen(serverfd, MAX_CONNECTIONS);
    if(ret < 0){
        Logger::ERROR("listen socket failed, err=%s\n", strerror(errno));
        return;
    }
    m_bRunning = true;
    //loop, 现在只是实现一个最简单的阻塞IO的
    while(m_bRunning){
        struct sockaddr_in clientaddr;
        socklen_t len;
        int clientsockfd = ::accept(serverfd, (struct sockaddr*)&clientaddr, &len);
        if(clientsockfd < 0){
            Logger::ERROR("accept client connection failed, err=%s\n", strerror(errno));
            continue;
        }
        Logger::INFO("New connection established from %s\n", addr2str(clientaddr).c_str());
        std::string buf;
        int n = readData(clientsockfd, buf);
        if(n<=0){
            Logger::ERROR("recv data from client failed, err=%s\n", strerror(errno));
            continue;
        }

        std::string response;
        handle_httpdata(buf, response);
        writeData(clientsockfd, response);
        ::close(clientsockfd);
    }
}

void CHttpServer::registerController(std::string & uriName, const CHttpServer::HttpCallback & cb) {
    m_controller.insert(make_pair(uriName, cb));
}

void CHttpServer::unregisterController(std::string & uriName) {
   auto it = m_controller.find(uriName);
   if(it != m_controller.end()){
       m_controller.erase(it);
   }
}

int CHttpServer::readData(int sockfd, std::string &buf) {
    char buffer[MAX_BUF_LEN];
    int n = ::read(sockfd, buffer, sizeof buffer);
    if(n<0){
        Logger::ERROR("read data from socket failed, err=%s\n", strerror(errno));
        return n;
    }
    buf = buffer;
    return n;
}

int CHttpServer::writeData(int sockfd, const std::string &buf) {
    int n = ::write(sockfd, buf.c_str(), buf.size());
    if(n<0){
        Logger::ERROR("write data to socket failed, err=%s\n", strerror(errno));
        return n;
    }
    return n;
}

void CHttpServer::handle_httpdata(const std::string &data, std::string &response) {
    Request request(data);
    if(!request.parse()){
        //bad request;
    }

    std::string uri = request.getUri();
    auto it = m_controller.find(uri);
    if(it==m_controller.end()){
        //404
    }

    HttpCallback cb = it->second;
    Response res = cb(request);
    response = res.toString();
}