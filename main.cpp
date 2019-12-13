//
// Created by gjk on 12/10/19.
//
#include "Logger.h"
#include "HttpServer.h"
#include "Response.h"
#include "Request.h"

Response Hello(const Request& request){
    Logger::INFO("Hello called\n");
    char hello_html[] = "<html>Hello World<html>";
    Response response(request.getHttpVersion(), 200, "OK");
    response.setBody(hello_html);
    return response;
}

int main(){
    char s[] = "GET /hello/niha HTTP/1.1\r\nHost: localhost:9984\r\nConnection: keep-alive\r\nCache-Control: max-age=0\r\nUpgrade-Insecure-Requests: 1\r\nUser-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/77.0.3865.90 Safari/537.36\r\nSec-Fetch-Mode: navigate\r\nSec-Fetch-User: ?1\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3\r\nSec-Fetch-Site: none\r\nAccept-Encoding: gzip, deflate, br\r\nAccept-Language: en-US,en;q=0.9\r\nCookie: Pycharm-b9c7b85f=02422951-c187-42dc-a6fe-43b36f83327d; session=.eJxdkVtrGzEQhf_KomfHaKSxLvsWEoNDaBtSF5qGYHSZjUXsXbPSFtqQ_16t2zw0T8PRnDnzMXplu-7g8p4yax9fWVNqYUfK2T0TW7C7A7lMzWF4blLflKFxIdRmU_YpN6fqWbKnt6dFDRkp71lbxomqSpG1TMXOaq9BkSGB6FBIAUpwrZHHDtCblebe26AB0DgnROdWHp1C1AEpehsFaMNDZ6SyhCQJTOg4eIWEnCvyngsu1Mp1UtqwwqAghgDKoIgOK_7uROPR9dSXdzQ_lJ7KdAa8-r0eXo4wiOlhHG8f7jHVkVCvkXNxZaoXqSrNwwtGfSzpSPVJcLAXABdgt6BbKVrApTVWavhRfakvdDhnVOtmu71r25vPu82Xr9vdZn15vb5_9_xD-NYZUaT__ivET7xPWdR-pvFnCvOu2OdZjyGdqgK-rLtgL";
    char ss[] = "POST /hello/world/?user=gjk&pass=123.. HTTP/1.1\r\nAccept: */*\r\nCache-Control: no-cache\r\nPfdsjk: cz342xbnm\r\nContent-Length: 40\r\nContent-Type: */*; charset=UTF-8\r\nHost: localhost:9981\r\nConnection: Keep-Alive\r\nUser-Agent: Apache-HttpClient/4.5.10 (Java/11.0.4)\r\nAccept-Encoding: gzip,deflate\r\n\r\nShou dao qing huifu, Thanks! Wo shi gjk.";
//    Logger::INFO("nihao %d, %f, %s,%d\n", 121,2.34f, "dsjak",23232233);
//    Logger::ERROR("nihao %d, %f, %s,%d\n", 121,2.34f, "dsjak",23232233);

    CHttpServer server(9982);
    server.registerController("/hello", Hello);
    server.start();
    return 0;
}
