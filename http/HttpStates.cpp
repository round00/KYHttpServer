//
// Created by gjk on 2020/3/3.
//

#include <vector>
#include "HttpStates.h"
#include "util.h"

REQUEST_METHOD getMethodByStr(const std::string& str){
    if(equalNocase(str, "GET")){
        return REQUEST_METHOD_GET;
    }else if(equalNocase(str, "POST")){
        return REQUEST_METHOD_POST;
    }else if(equalNocase(str, "HEAD")){
        return REQUEST_METHOD_HEAD;
    }else if(equalNocase(str, "PUT")){
        return REQUEST_METHOD_PUT;
    }else if(equalNocase(str, "DELETE")){
        return REQUEST_METHOD_DELETE;
    }else if(equalNocase(str, "OPTIONS")){
        return REQUEST_METHOD_OPTIONS;
    }else if(equalNocase(str, "TRACE")){
        return REQUEST_METHOD_TRACE;
    }else if(equalNocase(str, "CONNECT")){
        return REQUEST_METHOD_CONNECT;
    }
    return REQUEST_METHOD_UNKNOWN;
}

bool shouldHaveBody(REQUEST_METHOD method){
    switch (method){
        case REQUEST_METHOD_POST:
        case REQUEST_METHOD_PUT:
            return true;
        default:
            return false;
    }
}

//这些状态码都是从libevent拿过来的
const std::vector<std::vector<const char*>> PHRASES = {
        {},{
                /* 100 */ "Continue",
                /* 101 */ "Switching Protocols"
        },{
                /* 200 */ "OK",
                /* 201 */ "Created",
                /* 202 */ "Accepted",
                /* 203 */ "Non-Authoritative Information",
                /* 204 */ "No Content",
                /* 205 */ "Reset Content",
                /* 206 */ "Partial Content"
        },{
                /* 300 */ "Multiple Choices",
                /* 301 */ "Moved Permanently",
                /* 302 */ "Found",
                /* 303 */ "See Other",
                /* 304 */ "Not Modified",
                /* 305 */ "Use Proxy",
                          "Unknown Response Code",
                /* 307 */ "Temporary Redirect"
        },{
                /* 400 */ "Bad Request",
                /* 401 */ "Unauthorized",
                /* 402 */ "Payment Required",
                /* 403 */ "Forbidden",
                /* 404 */ "Not Found",
                /* 405 */ "Method Not Allowed",
                /* 406 */ "Not Acceptable",
                /* 407 */ "Proxy Authentication Required",
                /* 408 */ "Request Time-out",
                /* 409 */ "Conflict",
                /* 410 */ "Gone",
                /* 411 */ "Length Required",
                /* 412 */ "Precondition Failed",
                /* 413 */ "Request Entity Too Large",
                /* 414 */ "Request-URI Too Large",
                /* 415 */ "Unsupported Media Type",
                /* 416 */ "Requested range not satisfiable",
                /* 417 */ "Expectation Failed"
        },{
                /* 500 */ "Internal Server Error",
                /* 501 */ "Not Implemented",
                /* 502 */ "Bad Gateway",
                /* 503 */ "Service Unavailable",
                /* 504 */ "Gateway Time-out",
                /* 505 */ "HTTP Version not supported"
        }
};

const char* responsePhraseByCode(uint16_t code){
    if(code<100 || code>505){
        return "Unknown Response Code";
    }
    int i = code/100;     //1、2、3、4、5
    int j = code%100;
    int nsize = PHRASES[i].size();
    if(j >= nsize){
        return "Unknown Response Code";
    }
    return PHRASES[i][j];
}