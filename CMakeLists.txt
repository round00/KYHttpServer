cmake_minimum_required(VERSION 3.14)
project(KYHttpServer)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -g -Wall -O0")

include_directories(
    /usr/local/include/
    base/
    tcp/
    http/
)
link_directories(
    /usr/local/lib/
)

add_subdirectory(base)
add_subdirectory(tcp)
add_subdirectory(http)

add_executable(KYHttpServer main.cpp http/HttpStates.cpp)
target_link_libraries(KYHttpServer base tcp http)