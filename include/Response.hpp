#ifndef RESPONSE_H
#define RESPONSE_H
#include "HttpUtil.hpp"
#include <string>
#include <map>

class Response
{
private:
    std::string status;
    std::map<std::string,std::string> headers;
    std::string content;
public:
    Response();
    std::string getRawResponse() const;
};
#endif