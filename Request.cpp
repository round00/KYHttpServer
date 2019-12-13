//
// Created by gjk on 12/10/19.
//

#include <vector>
#include "Request.h"
#include "Logger.h"
#include "util.h"

bool Request::parse() {
    if(m_rawData.empty()){
        Logger::ERROR("http data is empty.\n");
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
        //check this situation /hello?user=gjk&pass=123
        pos = m_uri.find('?');
        if(pos!=-1){
            std::string params = m_uri.substr(pos+1, m_uri.length()-pos-1);
            m_uri = m_uri.substr(0, pos);
            std::vector<std::string> vecParam = spiltstring(params, '&');
            for(auto param:vecParam){
                pos = param.find('=');
                if(pos==-1)continue;
                std::string name = param.substr(0, pos);
                std::string value = param.substr(pos+1, param.length()-pos-1);
                m_headers[name] = value;
            }

        }
    }else{
        Logger::ERROR("http request line error.\n");
        return false;
    }

    if(vecStr[0] == "GET")m_requestMethod = METHOD_GET;
    else if(vecStr[0] == "POST")m_requestMethod = METHOD_POST;
    else if(vecStr[0] == "HEAD") m_requestMethod = METHOD_HEAD;
    else {
        Logger::ERROR("unknown http request method.\n");
        return false;
    }

    if(vecStr[2] == "HTTP/1.1")m_httpVersion = HTTP_1_1;
    else if(vecStr[2] == "HTTP/1.0")m_httpVersion = HTTP_1_0;
    else {
        Logger::ERROR("unknown http version.\n");
        return false;
    }

    //分割请求头和请求体
    pos = m_rawData.find("\r\n\r\n", beginPos); //请求头和请求体是以两个“\r\n”分开的
    std::string strHeader;
    if(pos == -1){
        //没有请求体
        strHeader = m_rawData.substr(beginPos, m_rawData.size() - beginPos);
    }else{
        strHeader = m_rawData.substr(beginPos, pos-beginPos);
        pos += 4;
        m_body = m_rawData.substr(pos, m_rawData.size() - pos);
    }

    //解析请求头
    vecStr = spiltstring(strHeader, "\r\n");
    for(const auto& str : vecStr){
        pos = str.find(':');
        if(pos==-1)continue;

        std::string name = str.substr(0, pos);
        std::string value = str.substr(pos+1, str.length()-pos-1);
        m_headers[name] = value;
    }
    return true;
}

std::string Request::getValue(std::string& key) {
    auto it = m_headers.find(key);
    if(it == m_headers.end()){
        return "";
    }

    return it->second;
}