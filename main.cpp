//
// Created by gjk on 12/10/19.
//
#include "Logger.h"
#include "HttpServer.h"
int main(){

//    Logger::INFO("nihao %d, %f, %s,%d\n", 121,2.34f, "dsjak",23232233);
//    Logger::ERROR("nihao %d, %f, %s,%d\n", 121,2.34f, "dsjak",23232233);
    CHttpServer server(9984);
    server.start();
    return 0;
}