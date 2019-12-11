//
// Created by gjk on 12/10/19.
//

#include <vector>
#include "Request.h"
#include "Logger.h"
#include "util.h"

bool Request::parse() {
    if(m_rawData.empty()){
        Logger::ERROR("http data is empty.");
        return false;
    }

    int beginPos = 0, pos = 0;

    //解析请求行： GET /hello/niha HTTP/1.1
    pos = m_rawData.find("\r\n", beginPos);
    std::string requestLine = m_rawData.substr(beginPos, pos-beginPos);
    beginPos = pos + 2;

    std::vector<std::string> vecStr = spiltstring(requestLine, ' ');
    if(vecStr.size() == 2){
        m_uri = "/";
    }else if(vecStr.size()==3){
        m_uri = vecStr[1];
    }else{
        Logger::ERROR("http request line error.");
        return false;
    }

    if(vecStr[0] == "GET")m_requestMethod = METHOD_GET;
    else if(vecStr[0] == "POST")m_requestMethod = METHOD_POST;
    else if(vecStr[0] == "HEAD") m_requestMethod = METHOD_HEAD;
    else {
        Logger::ERROR("unknown http request method.");
        return false;
    }

    if(vecStr[2] == "HTTP/1.1")m_httpVersion = HTTP_1_1;
    else if(vecStr[2] == "HTTP/1.0")m_httpVersion = HTTP_1_0;
    else {
        Logger::ERROR("unknown http version.");
        return false;
    }

    //分割请求头和请求体
    pos = m_rawData.find("\r\n\r\n", beginPos); //请求头和请求体是以两个“\r\n”分开的
    std::string strHeader, strBody;
    if(pos == -1){
        //没有请求体
        strHeader = m_rawData.substr(beginPos, m_rawData.size() - beginPos);
    }else{
        strHeader = m_rawData.substr(beginPos, pos-beginPos);
        pos += 3;
        strBody = m_rawData.substr(m_rawData.size() - pos);
    }

    //解析请求头
    vecStr = spiltstring(strHeader, );




}

std::string Request::getValue(std::string& key) {
    auto it = m_headers.find(key);
    if(it == m_headers.end()){
        return "";
    }

    return it->second;
}