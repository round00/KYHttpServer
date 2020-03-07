set(tcp_src
        Buffer.cpp
        TcpConnection.cpp
        TcpServer.cpp
        Epoller.cpp
        Event.cpp
        EventLoop.cpp
        SocketUtil.cpp)

add_library(tcp ${tcp_src})
target_link_libraries(tcp base)
