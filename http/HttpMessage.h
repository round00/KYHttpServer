//
// Created by gjk on 2020/3/3.
// Http报文类
//

#ifndef KYHTTPSERVER_HTTPMESSAGE_H
#define KYHTTPSERVER_HTTPMESSAGE_H

#include <string>
#include <unordered_map>
#include <stdint.h>
#include <vector>
#include "HttpStates.h"
using std::string;

class CHttpMessage{
public:
    CHttpMessage();

    void                setHttpVersion(uint8_t major=1, uint8_t minor=1)
    {m_httpVersionMajor = major;m_httpVersionMinor = minor;}
    //返回http版本，用对应的版本*10表示，例如HTPP/1.1 = 11
    uint8_t             getHttpVersion();
    void                addHeader(const string& key, const string& val);
    string              getHeader(const string& key);
    void                delHeader(const string& key);
    std::vector<std::pair<string,string>>
                        getAllHeaders();

    void                setBody(const string& body){m_body = body;}
    string              getBody(){return m_body;}
    void                addBodyChunk(const string& body){m_body.append(body);}

    void                setBodyType(BODY_TYPE bt){m_bodyType = bt;}
    BODY_TYPE           getBodyType(){return m_bodyType;}

    void                setBodyLen(int len){m_bodyLen = len;}
    int                 getBodyLen(){return m_bodyLen;}

    void                setBodyChunkSize(int sz){m_bodyChunkSize = sz;}
    int                 getBodyChunkSize(){return m_bodyChunkSize;}

private:
    //Http版本号
    uint8_t             m_httpVersionMajor;
    uint8_t             m_httpVersionMinor;

    //报文头
    std::unordered_map<string, string>
                        m_headers;
    //报文主体
    string              m_body;
    //按照什么类型读取body
    BODY_TYPE           m_bodyType;
    //主体长度，只有在bodyType==BODY_TYPE_LENGTH时有效
    int                 m_bodyLen;
    //主体当前分块的长度，只有在bodyType==BODY_TYPE_CHUNK时有效,
    // <0表示当前需要读取分块长度
    int                 m_bodyChunkSize;

};

#endif //KYHTTPSERVER_HTTPMESSAGE_H
