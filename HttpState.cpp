//
// Created by gjk on 12/13/19.
//
#include "HttpState.h"

//char *getHttpRequestMethodStr(HTTP_REQUEST_METHOD method){
//    if(method == METHOD_GET)return "GET";
//    else if(method == METHOD_HEAD)return "HEAD";
//    else if(method == METHOD_POST)return "POST";
//    else return "UNKNOWN_METHOD";
//}

std::string getHttpVersionStr(HTTP_VERSION version){
    if(version == HTTP_1_0)return "HTTP/1.0";
    else if (version == HTTP_1_1)return "HTTP/1.1";
    else return "UNKNOWN_VERSION";
}