set(http_src
        HttpConnection.cpp
        HttpMessage.cpp
        HttpRequest.cpp
        HttpResponse.cpp
        HttpServer.cpp
        HttpStates.cpp
        )

add_library(http ${http_src})
#target_link_libraries(base pthread)
