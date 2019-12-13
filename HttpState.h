//
// Created by gjk on 12/11/19.
//

#ifndef KYHTTPSERVER_HTTPSTATE_H
#define KYHTTPSERVER_HTTPSTATE_H

#include <string>
enum HTTP_REQUEST_METHOD{
    METHOD_HEAD = 0,
    METHOD_GET,
    METHOD_POST
};
//char *getHttpRequestMethodStr(HTTP_REQUEST_METHOD method);

enum HTTP_VERSION{
    HTTP_1_0 = 0,
    HTTP_1_1
};
std::string getHttpVersionStr(HTTP_VERSION version);



#endif //KYHTTPSERVER_HTTPSTATE_H
