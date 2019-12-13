//
// Created by gjk on 12/10/19.
//
#include "Response.h"


Response::Response(HTTP_VERSION version, int status, const std::string& msg)
    :m_httpVersion(version),
    m_statusCode(status),
    m_msg(msg)
{
    if(status==404){
        setBody("<html>There is no that page!<html>");
    }else if(status==400){
        setBody("<html>Check your request<html>");
    }
}

std::string Response::toString() {
    //add response line
    std::string result;
    result += getHttpVersionStr(m_httpVersion);
    result += " ";
    result += std::to_string(m_statusCode);
    result += " ";
    result += m_msg;
    result += "\r\n";

    //add headers
    if(m_headers.find("Content-Length") == m_headers.end()){
        m_headers["Content-Length"] = "0";
    }
    for(auto it : m_headers){
        result += it.first + ": " +it.second + "\r\n";
    }

    //add body;
    result += "\r\n";
    result += m_body;
    return result;
}

void Response::addHeaders(const std::string & key, const std::string & value) {
    m_headers[key] = value;
}

void Response::setBody(const std::string& body) {
    if(body.empty())return;

    int len = body.length();
    addHeaders("Content-Length", std::to_string(len));
    m_body = body;
}