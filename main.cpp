//
// Created by gjk on 12/10/19.
//

#include <cstring>
#include <string>
#include <stdio.h>
#include "HttpServer.h"
#include "Logger.h"
using namespace std;

const char* basedir = "/tmp/httpserver";
void defaultController(const HttpRequestPtr& request, HttpResponsePtr response){
    char filename[256];
    strcpy(filename, basedir);
    strcat(filename, "/index.html");
    FILE*pfile = fopen(filename, "rb");
    char buf[65536];
    auto nread = fread(buf, 1, 65536, pfile);
    if(nread > 0){
        string s(buf, nread);
        response->setBody(s);
        response->addHeader("Content-type", "text/html");
//        response->addHeader("Content-Length", to_string(s.length()));
        response->setResponseLine(200, "OK");
    }
    fclose(pfile);
}

void helloController(const HttpRequestPtr& request, HttpResponsePtr response){
    char filename[256];
    strcpy(filename, basedir);
    strcat(filename, "/hello.ico");
    FILE*pfile = fopen(filename, "rb");
    char buf[65536];
    auto nread = fread(buf, 1, 65536, pfile);
    if(nread > 0){
        string s(buf, nread);
        response->setBody(s);
        response->addHeader("Content-type", "image/png");
        response->addHeader("Content-Length", to_string(s.length()));
        response->setResponseLine(200, "OK");
    }
    fclose(pfile);
}

int main(){

    if(!CLogger::init("logs/")){
        fprintf(stderr, "Logger init failed\n");
        return 0;
    }

    CHttpServer server(9876, 1);
    server.addController("/hello", helloController);
    server.setDefaultController(defaultController);
    if(!server.init()){
        fprintf(stderr, "Init httpserver failed\n");
        return (0);
    }

    server.loop();
    return 0;
}
