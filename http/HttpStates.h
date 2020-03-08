//
// Created by gjk on 2020/3/3.
//

#ifndef KYHTTPSERVER_HTTPSTATES_H
#define KYHTTPSERVER_HTTPSTATES_H

#include <stdint.h>
#include <string>

const int MAX_HEADER_LEN = 4096;

//http请求方法
//用位运算可以方便的设置和获取支持的http方法
enum REQUEST_METHOD{
    REQUEST_METHOD_UNKNOWN,
    REQUEST_METHOD_GET     = 1u << 0u,
    REQUEST_METHOD_POST    = 1u << 1u,
    REQUEST_METHOD_HEAD    = 1u << 2u,
    REQUEST_METHOD_PUT     = 1u << 3u,
    REQUEST_METHOD_DELETE  = 1u << 4u,
};
//默认支持下面这些方法
const uint16_t DEFAULT_METHOD =
    REQUEST_METHOD_GET|REQUEST_METHOD_POST|REQUEST_METHOD_HEAD;
REQUEST_METHOD getMethodByStr(const std::string& str);
//根据方法判断是否需要有一个主体
bool shouldHaveBody(REQUEST_METHOD method);

//由http响应码获取对应的原因短语
const char* responsePhraseByCode(uint16_t code);

//当前连接的状态
enum CONNECTION_STATE{
    CONN_DISCONNECTED,  //未连接
    CONN_CONNECTED,     //已连接
    CONN_READ_FIRSTLINE,//正在读取请求的第一行
    CONN_READ_HEADERS,  //正在读取请求头
    CONN_READ_BODY,     //正在读取请求体
    CONN_READ_DONE,     //请求读取完成，准备分发请求
    CONN_WRITING        //正在写，这个就是写给客户端的响应
};

//body是以什么样的形式传过来的
enum BODY_TYPE{
    BODY_TYPE_UNSURE,   //还不确定
    BODY_TYPE_LENGTH,   //以Content-Length字段指定
    BODY_TYPE_CHUNKED,  //以Transfer-Encoding=chunked指定
    BODY_TYPE_NOBODY,   //没有主体
};

//请求失败的原因
enum REQUEST_FAILED{
    REQUEST_FAILED_FIRSTLINE,   //读取第一行的时候出错
    REQUEST_FAILED_HEADER,      //读取报文头的时候出错
    REQUEST_FAILED_BODY,        //读取主体的时候出错
    REQUEST_FAILED_INTERNAL     //服务端内部错误

};

#endif //KYHTTPSERVER_HTTPSTATES_H
