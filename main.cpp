//
// Created by gjk on 12/10/19.
//

#include <cstring>
#include <string>
#include <stdio.h>
#include "HttpServer.h"
#include "Logger.h"
using namespace std;
char page[1024];
string spage;
const char* basedir = "/tmp/httpserver";
void defaultController(const HttpRequestPtr& request, HttpResponsePtr response){


    response->setBody(spage);
    response->addHeader("Content-type", "text/plain");
    response->addHeader("Content-Length", to_string(spage.length()));
    response->setResponseLine(200, "OK");
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

    char filename[256];
    strcpy(filename, basedir);
    strcat(filename, "/hello.html");
    FILE*pfile = fopen(filename, "rb");
    fread(page, 1, sizeof(page), pfile);
    fclose(pfile);
    spage = page;

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
