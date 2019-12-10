cmake_minimum_required(VERSION 3.15)
project(KYHttpServer)

set(LIBSRC
        Logger.cpp
        HttpServer.cpp
        Request.h
        Response.cpp
)

add_executable(main main.cpp ${LIBSRC})