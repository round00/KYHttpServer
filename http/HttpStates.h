//
// Created by gjk on 2020/3/3.
//

#ifndef KYHTTPSERVER_HTTPSTATES_H
#define KYHTTPSERVER_HTTPSTATES_H

#include <stdint.h>

//http请求方法
//用位运算可以方便的设置和获取支持的http方法
enum REQUEST_METHOD{
    REQUEST_METHOD_GET     = 1u << 0u,
    REQUEST_METHOD_POST    = 1u << 1u,
    REQUEST_METHOD_HEAD    = 1u << 2u,
    REQUEST_METHOD_PUT     = 1u << 3u,
    REQUEST_METHOD_DELETE  = 1u << 4u,
    REQUEST_METHOD_OPTIONS = 1u << 5u,
    REQUEST_METHOD_TRACE   = 1u << 6u,
    REQUEST_METHOD_CONNECT = 1u << 7u,
};

//由http响应码获取对应的原因短语
const char* responsePhraseByCode(uint16_t code);

#endif //KYHTTPSERVER_HTTPSTATES_H
