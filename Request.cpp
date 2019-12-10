//
// Created by gjk on 12/10/19.
//

#include "Request.h"

bool Request::parse() {
    return true;
}

std::string Request::getValue(std::string& key) {
    auto it = m_headers.find(key);
    if(it == m_headers.end()){
        return "";
    }

    return it->second;
}