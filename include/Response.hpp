#ifndef RESPONSE_H
#define RESPONSE_H
#include "HttpUtil.hpp"

#include <string>
#include <sstream>
#include <map>

class Response
{
private:
    std::string http_ver;
    std::string status;
    std::map<std::string, std::string> headers;
    std::stringstream content;

public:
    Response();
    virtual void setHeader(const std::string &, const std::string &);
    virtual std::string getHeader(const std::string &);
    virtual void writeContent(const std::string &);
    virtual std::string getStatusLine();
    virtual std::stringstream &getContentStream();
    virtual const std::map<std::string, std::string> &getHeaders();
};
#endif