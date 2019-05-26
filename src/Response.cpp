#include "Response.hpp"

#include <ctime>

Response::Response()
{
    //Set default status
    http_ver = HTTP_VER " ";
    status = HTTP_STATUS_OK "\r\n";
    //Set date header with current time
    std::time_t time = std::time(0);
    tm time_tm;
    gmtime_s(&time_tm, &time);
    char buff[128];
    strftime(buff, 128, "%a, %d %b %Y %H:%M:%S GMT", &time_tm);
    headers["Date"] = std::string(buff);
}

std::string Response::getStatusLine()
{
    return http_ver + status;
}

std::stringstream &Response::getContentStream()
{
    return content;
}

void Response::setHeader(const std::string &hName, const std::string &hValue)
{
    headers[hName] = hValue;
}

std::string Response::getHeader(const std::string &hName)
{
    return headers[hName];
}

void Response::writeContent(const std::string &newContent)
{
    content << newContent;
    headers["Content-Lenght"] = std::to_string(content.tellp());
}

const std::map<std::string, std::string> &Response::getHeaders()
{
    return headers;
}
