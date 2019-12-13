cmake_minimum_required(VERSION 3.15)
project(KYHttpServer)
set(CMAKE_CXX_STANDARD 11)

set(LIBSRC
        Logger.cpp
        HttpServer.cpp
        Request.cpp
        Response.cpp
        util.cpp
        HttpState.cpp
)

add_executable(main main.cpp ${LIBSRC})