//
// Created by gjk on 2020/3/3.
//
#include <vector>
#include "HttpConnection.h"
#include "Logger.h"
#include "util.h"

CHttpConnection::CHttpConnection()
:m_request(nullptr), m_response(nullptr),
m_connState(CONN_DISCONNECTED){

}

void CHttpConnection::onNewRequest(TcpConnPtr tcpConn, BufferPtr buffer) {
    if(m_connState == CONN_DISCONNECTED || m_connState == CONN_WRITING)
        return;
    if(m_connState == CONN_CONNECTED){  //第一次连接
        //关闭tcp连接的写
        tcpConn->enableWrite(false);
        m_tcpConn = tcpConn;
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
    //到这里这个请求就算是处理完了

    bool needClose = false;
    string Connection = m_request->getHeader("Connection");
    if(m_request->getHttpVersion()==10 &&//HTTP/1.0
        !equalNocase(Connection, "Keep-Alive")){    //没有Keep-Alive
        needClose = true;
    }else if(m_request->getHttpVersion()==11 && //HTTP/1.1
            equalNocase(Connection, "close")){
        needClose = true;
    }

    resetHttpConnection();
    if(needClose)
        tcpConn->close();
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
    auto items = split(line);
    if(items.size() != 3){//HTTP请求的第一行要有三个参数
        requestFailed(REQUEST_FAILED_FIRSTLINE);
        return;
    }
    m_request->setMethod(getMethodByStr(items[0]));
    m_request->setUri(items[1]);
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
        auto items = split(header, ':');
        //报文头的每一行应该是用:分开的键值对
        if(items.size()!=2){
            requestFailed(REQUEST_FAILED_HEADER);
            return;
        }
        string key = items[0];
        string val = items[1];
        m_request->addHeader(key, val);
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
    if(buffer->readAbleLength() < nBody){
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
            if(buffer->readAbleLength() < chunkSzie){
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

//void CHttpConnection::requestFailed(REQUEST_FAILED reason) {
//
//}

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
    requestDispath();
    //请求分发完，现在应该将响应发送给客户端了
    responseClient();
}

void CHttpConnection::requestDispath() {
    auto controller = getController(m_request->getUri());
    m_response.reset(new CHttpResponse());
    if(controller){    //没有任何合适的回调
        m_response->setResponseLine(404, "Not Found");
        return;
    }

    controller(m_request, m_response);
}

Controller CHttpConnection::getController(const string &uri) {
    auto server = m_httpServer.lock();
    if(!server){
        return nullptr;
    }
    auto cc = server->getController(uri);
    if(cc)
        return cc;
    cc = server->getDefaultController();
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

void CHttpConnection::resetHttpConnection() {
    m_request.reset();
    m_response.reset();
    m_connState = CONN_DISCONNECTED;
}