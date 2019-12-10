//
// Created by gjk on 12/10/19.
//

#ifndef KYHTTPSERVER_HTTPSERVER_H
#define KYHTTPSERVER_HTTPSERVER_H

#include <string>
#include <unordered_map>
#include <functional>

class Request;
class Response;

class CHttpServer{
public:
    typedef std::function<Response(const Request& request)>  HttpCallback;
    const int MAX_CONNECTIONS = 16;
    CHttpServer(int port);
    void start();

    void registerController(std::string& uriName, const HttpCallback& cb);
    void unregisterController(std::string& uriName);
private:

    int readData(int sockfd, std::string& buf);
    int writeData(int sockfd, const std::string& buf);
    void handle_httpdata(const std::string& data, std::string& response);
    int m_port;
    bool m_bRunning;
    std::unordered_map<std::string, const HttpCallback&> m_controller;
};
#endif //KYHTTPSERVER_HTTPSERVER_H
