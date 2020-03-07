//
// Created by gjk on 2020/3/3.
//
#include <vector>
#include "HttpConnection.h"
#include "Logger.h"
#include "util.h"
#include "HttpServer.h"

CHttpConnection::CHttpConnection(CHttpServer* httpServer)
:m_httpServer(httpServer), m_request(nullptr), m_response(nullptr),
m_connState(CONN_DISCONNECTED){

}

void CHttpConnection::onNewRequest(TcpConnPtr tcpConn, BufferPtr buffer) {
    if(m_connState == CONN_DISCONNECTED || m_connState == CONN_WRITING)
        return;
    if(m_connState == CONN_CONNECTED){  //第一次收到数据
        //关闭tcp连接的写
        tcpConn->enableWrite(false);
        //创建一个http请求，等会填充
        m_request.reset(new CHttpRequest());
        m_connState = CONN_READ_FIRSTLINE;
    }
    //走到这里状态只能是以下三种了
    //因为一个http请求可能在一次回调中的buffer中，还没有拥有整个http请求的数据
    //所以分阶段来读
    switch (m_connState){
        case CONN_READ_FIRSTLINE:
            readFirstLine(buffer);
            break;
        case CONN_READ_HEADERS:
            readHeaders(buffer);
            break;
        case CONN_READ_BODY:
            readBody(buffer);
            break;
        default:
            LOGE("The http connection state is invalid(%d) now", m_connState);
            break;
    }
}

void CHttpConnection::onResponseDone(TcpConnPtr tcpConn) {
    //到这里这个请求就算是处理完了，如果不需要关闭就坐等下一次请求到来就好了
    bool needClose = connNeedClose();

    resetHttpConnection();
    if(needClose)
        tcpConn->closeConnection();
}

void CHttpConnection::readFirstLine(BufferPtr buffer) {
    if(m_connState != CONN_READ_FIRSTLINE){
        return;
    }
    std::string line;
    int nRead = buffer->readALine(line);
    if(nRead<0){//数据还不够一行
        LOGI("Read first line failed, maybe data is not enough");
        return;
    }
    if(nRead==0){//数据读成功了，但是第一行没有数据，请求数据有错
        requestFailed(REQUEST_FAILED_FIRSTLINE);
        return;
    }
    auto items = spilt(line);
    if(items.size() != 3){//HTTP请求的第一行要有三个参数
        requestFailed(REQUEST_FAILED_FIRSTLINE);
        return;
    }
    m_request->setMethod(getMethodByStr(items[0]));
    m_request->parseUri(items[1]);
    //读取http的版本号
    int major = 1, minor = 1;
    if(sscanf(items[2].c_str(), "HTTP/%d.%d", &major, &minor)!=2
        ||major!=1){
        requestFailed(REQUEST_FAILED_FIRSTLINE);
        return;
    }
    m_request->setHttpVersion(major, minor);
    //请求行读取完成，接下来该读头部了
    m_connState = CONN_READ_HEADERS;
    readHeaders(buffer);
}

void CHttpConnection::readHeaders(BufferPtr buffer) {
    if(m_connState!=CONN_READ_HEADERS){
        return;
    }
    std::string header;
    int nRead = 0;
    while((nRead = buffer->readALine(header)) > 0){
        //报文头的每一行应该是用:分开的键值对
        auto pos = header.find(':');
        if(pos == string::npos){
            continue;
        }
        string key = header.substr(0, pos);
        string val = header.substr(pos+1);
        m_request->addHeader(trim(key), trim(val));
    }

    if(nRead < 0){
        LOGI("Read header line failed, maybe data is not enough");
        return;
    }

    if(nRead==0){//说明读了一个空行，即\r\n，也就是请求头读完了
        m_connState = CONN_READ_BODY;
        readBody(buffer);
    }
}

void CHttpConnection::readBody(BufferPtr buffer) {
    if(m_connState!=CONN_READ_BODY){
        return;
    }
    //处理主体长度有两种方式：1、Content-Length，2：Transfer-Encoding=chunked
    if(m_request->getBodyType() == BODY_TYPE_LENGTH){
        readBodyByLength(buffer);
    } else if(m_request->getBodyType() == BODY_TYPE_CHUNKED){
        readBodyByChunk(buffer);
    } else if(m_request->getBodyType() == BODY_TYPE_NOBODY){
        requestSucess();
    } else if(m_request->getBodyType() == BODY_TYPE_UNSURE){
        do{ //1、Content-Length
            string length = m_request->getHeader("Content-Length");
            if(length.empty())
                break;
            int nBody = atoi(length.c_str());
            if(nBody<=0)
                break;
            m_request->setBodyType(BODY_TYPE_LENGTH);
            m_request->setBodyLen(nBody);
            readBodyByLength(buffer);
            return;
        }while(false);

        do{ //2：Transfer-Encoding=chunked
            string chunk = m_request->getHeader("Transfer-Encoding");
            if(chunk.empty() || !equalNocase(chunk, "chunked"))
                break;
            //到这里就是需要分块读了
            m_request->setBodyType(BODY_TYPE_CHUNKED);
            m_request->setBodyChunkSize(-1);
            readBodyByChunk(buffer);
            return;
        }while(false);
        //到这里说明没有主体
        if(shouldHaveBody(m_request->getMethod())){
            //如果应该有主体但是没有，则报错
            requestFailed(REQUEST_FAILED_BODY);
            return;
        }
        //剩下的就是没有主体的情况了
        m_request->setBodyType(BODY_TYPE_NOBODY);
        requestSucess();
    }else{
        LOGE("Read body: Unknown body type");
        requestFailed(REQUEST_FAILED_INTERNAL);
    }
}

void CHttpConnection::readBodyByLength(BufferPtr buffer) {
    if(m_connState != CONN_READ_BODY || m_request->getBodyType()!=BODY_TYPE_LENGTH)
        return;
    int nBody = m_request->getBodyLen();
    if(nBody <= 0){
        LOGE("Read body by length: nBody <= 0");
        requestFailed(REQUEST_FAILED_INTERNAL);
        return ;
    }

    //使用Content-Length字段指定了长度
    if(buffer->readableLength() < nBody){
        //数据还不够，再等等
        return;
    }

    std::string body;
    int nRead = buffer->readNbytes(body, nBody);
    if(nRead<nBody){
        LOGE("Read body by length: read from buffer failed");
        requestFailed(REQUEST_FAILED_INTERNAL);
        return;
    }
    m_request->setBody(body);
    requestSucess();
}

void CHttpConnection::readBodyByChunk(BufferPtr buffer) {
    if(m_connState != CONN_READ_BODY || m_request->getBodyType()!=BODY_TYPE_CHUNKED)
        return;
/*
11\r\n
Hello world\r\n
0\r\n
\r\n
*/
    while(true){
        int chunkSzie = m_request->getBodyChunkSize();
        if(chunkSzie < 0){    //需要先读分块长度
            string line;
            int nRead = buffer->readALine(line);
            if(nRead < 0){  //说明现在还不够一行,要等数据够了再继续
                return;
            }
            if(nRead == 0){ //这就是读到最后一个\r\n了
                break;
            }
            chunkSzie = atoi(line.c_str());
            if(chunkSzie == 0){ //块长度为0，是最后一个块，后面再有一个\r\n就读完了
                continue;
            }
            if(chunkSzie < 0){  //出现错误了
                requestFailed(REQUEST_FAILED_BODY);
                return;
            }
            m_request->setBodyChunkSize(chunkSzie);
        }else{  //该读分块了
            chunkSzie+=2;   //+2是指\r\n的长度
            if(buffer->readableLength() < chunkSzie){
                //数据不够
                return;
            }

            string chunk;
            //连\r\n也读出来
            int nRead = buffer->readNbytes(chunk, chunkSzie);
            if(nRead < chunkSzie){
                requestFailed(REQUEST_FAILED_INTERNAL);
                return ;
            }
            //把\r\n去掉
            chunk.pop_back();
            chunk.pop_back();
            m_request->addBodyChunk(chunk);
            m_request->setBodyChunkSize(-1);
        }
    }
    requestSucess();
}

void CHttpConnection::requestFailed(REQUEST_FAILED reason) {
    if(reason == REQUEST_FAILED_FIRSTLINE ||
        reason == REQUEST_FAILED_HEADER ||
        reason == REQUEST_FAILED_BODY){
        responseClientError(400, "Bad Request");
    }else {
        responseClientError(500, "Internal Server Error");
    }
}

void CHttpConnection::requestSucess() {
    m_connState = CONN_READ_DONE;
    auto tcpConn = m_tcpConn.lock();
    if(!tcpConn){
        //如果tcp连接都断了，那还玩啥啊
        LOGE("request success, but tcp is disconnected");
        return;
    }
    //不在接收数据了
    tcpConn->enableRead(false);
    //分发请求
    if(requestDispath()){
        //请求分发完，现在应该将响应发送给客户端了
        responseClient();
    }
}

bool CHttpConnection::requestDispath() {
    auto controller = getController(m_request->getUri());
    m_response.reset(new CHttpResponse());
    if(!m_httpServer){
        requestFailed(REQUEST_FAILED_INTERNAL);
        return false;
    }
    if(!(m_httpServer->getAllowMethod() & m_request->getMethod())){
        responseClientError(501, "Not Implemented");
        return false;
    }

    if(!controller){    //没有任何合适的回调
        responseClientError(404, "Not Found");
        return false;
    }

    controller(m_request, m_response);
    return true;
}

Controller CHttpConnection::getController(const string &uri) {
    if(!m_httpServer){
        return nullptr;
    }
    auto cc = m_httpServer->getController(uri);
    if(cc)
        return cc;
    cc = m_httpServer->getDefaultController();
    return cc;
}

void CHttpConnection::responseClient() {
    m_connState = CONN_WRITING;
    do{
        if(!m_response)
            break;
        auto tcpConn = m_tcpConn.lock();
        if(!tcpConn)
            break;
        string packet = makeResponsePacket();
        //设置响应发送完的回调
        tcpConn->setWriteCompleteCb(std::bind(&CHttpConnection::onResponseDone,
                                           this, std::placeholders::_1));
        tcpConn->enableWrite(true);
        tcpConn->send(packet);
    }while(false);

}

static const char* ERR_FORMAT =
    "<HTML>"
        "<HEAD>\n"
	        "<TITLE>%d %s</TITLE>\n"
	    "</HEAD>"
        "<BODY>\n"
	        "<H1>%s</H1>\n"
	    "</BODY>"
        "<hr><em>KYHttpServer</em>"
     "</HTML>\n";
void CHttpConnection::responseClientError(int code, const string &reason) {
    if(!m_response)
        m_response.reset(new CHttpResponse());

    m_response->setResponseLine(code, reason);
    m_response->addHeader("Content-Type", "text/html");
    m_response->addHeader("Connection", "close");

    char buf[256];
    snprintf(buf, 256, ERR_FORMAT, code, reason.c_str(), reason.c_str());
    m_response->setBody(buf);
    responseClient();
}

string CHttpConnection::makeResponsePacket() {
    if(!m_response)return "";
    string packet;
    char buf[MAX_HEADER_LEN];
    //响应行
    uint16_t code = m_response->getResponseCode();
    string reason = m_response->getResponseReason();
    //没有设置响应码，
    if(code==0)code = 200;
    //没有设置原因短语
    if(reason.empty())reason = responsePhraseByCode(code);

    int major, minor;
    major = m_response->getHttpVersion();
    minor = major%10;
    major /= 10;
    snprintf(buf, MAX_HEADER_LEN, "HTTP/%d.%d %d %s\r\n", major, minor, code, reason.c_str());
    packet += buf;

    //响应头
    //先把用户设置的所有头加进去
    auto headers = m_response->getAllHeaders();
    for(const auto& header:headers){
        header2Str(packet, header.first, header.second);
    }

    //再加一些用户没有设置的头
    if(m_response->getHeader("Host").empty()){
        header2Str(packet, "Host", "KYHttpServer");
    }
    //看下是否需要加Connection字段
    if(m_response->getHeader("Connection").empty()){
        if(!connNeedClose()){
            if(m_request->getHttpVersion()==10)
                header2Str(packet, "Connection", "keep-alive");
        }else{
            if(m_request->getHttpVersion()==11)
                header2Str(packet, "Connection", "close");
        }
    }else {//响应设置了是否close，把它也写到请求中，因为之后要根据请求中的字段来判断是否要关闭连接
        m_request->addHeader("Connection", "close");
    }
    string body = m_response->getBody();
    if(m_response->getHeader("Content-Length").empty())
        header2Str(packet, "Content-Length", std::to_string(body.size()));

    //头部添加完了，加一个CRLF
    packet += "\r\n";
    //添加主体
    packet += body;
    return packet;
}

void CHttpConnection::resetHttpConnection() {
    m_request.reset();
    m_response.reset();
    m_connState = CONN_CONNECTED;
    auto tcpConn = m_tcpConn.lock();
    if(tcpConn){
        tcpConn->getInputBuffer()->drainAll();
        tcpConn->getOutputBuffer()->drainAll();
        tcpConn->enableWrite(false);
        tcpConn->enableRead(true);
    }
}

void CHttpConnection::header2Str(string &str, const string &key, const string &val) {
    str += key;
    str += ": ";
    str += val;
    str += "\r\n";
}

bool CHttpConnection::connNeedClose() {
    if(!m_request)return false;
    string Connection = m_request->getHeader("Connection");
    if(m_request->getHttpVersion()==10 &&//HTTP/1.0
       !equalNocase(Connection, "Keep-Alive")){    //没有Keep-Alive
        return true;
    }else if(m_request->getHttpVersion()==11 && //HTTP/1.1
             equalNocase(Connection, "close")){
        return true;
    }
    return false;
}