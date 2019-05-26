#include "Request.hpp"

#include <iostream>
#include <sstream>
#include <vector>

Request::Request(std::string const &sRequest)
{
    std::istringstream inputStream(sRequest);
    std::vector<std::string> lines;
    //Read and split request into lines
    std::string lineBuf;
    while (std::getline(inputStream, lineBuf))
    {
        lines.push_back(lineBuf);
    }
    //Get method and request url from the first line
    method = lines[0].substr(lines[0].find_first_of(' '));
    url = lines[0].substr(lines[0].find_first_of(' '), lines[0].find_last_of(' '));
    //Get headers
    for (size_t i = 0; i < lines.size(); i++)
    {
        auto colonIndex = lines[i].find_first_of(':');
        if (colonIndex == std::string::npos)
            continue;
        std::string header = lines[i].substr(0, colonIndex);
        std::string value = lines[i].substr(colonIndex, lines[i].length());
        headers[header] = value;
    }
}

std::string Request::getHeader(std::string const &header)
{
    return headers[header];
}

std::string Request::getMethod()
{
    return method;
}

std::string Request::getParameter(std::string const &)
{
    return std::string();
}
