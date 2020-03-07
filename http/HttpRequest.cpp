//
// Created by gjk on 2020/3/3.
//

#include "HttpRequest.h"
#include "util.h"

CHttpRequest::CHttpRequest()
:m_method(REQUEST_METHOD_GET)
{

}

void CHttpRequest::parseUri(const string& uri) {
    if(uri.empty())return;
    int last = uri.length();
    //从后往前，先解析最后的fragment，由井号开始
    auto pos = uri.find('#');
    if(pos!=string::npos){
        m_fragment = uri.substr(pos, last-pos);
        last = pos;
    }
    //再解析query，即 name=gjk&pass=123 段
    pos = uri.find('?');
    if(pos != string::npos){
        string queryseg = uri.substr(pos, last);

        auto querys = spilt(queryseg, '&');
        for(const auto& query:querys){
            auto header = spilt(query, '=');
            if(header.empty())
                continue;
            addHeader(header[0], header[1]);
        }
        last = pos;
    }
    //剩下的就是uri了
    m_uri = uri.substr(0, last);
}