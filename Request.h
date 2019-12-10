//
// Created by gjk on 12/10/19.
//

#ifndef KYHTTPSERVER_REQUEST_H
#define KYHTTPSERVER_REQUEST_H

#include <string>
#include <unordered_map>

class Request{
public:
    Request(const std::string& data):m_rawData(data){}
    bool parse();
    std::string getUri(){return m_uri;}
    std::string getValue(std::string& key);
private:
    std::string m_rawData;
    std::string m_uri;
    std::unordered_map<std::string, std::string> m_headers;
};
#endif //KYHTTPSERVER_REQUEST_H
