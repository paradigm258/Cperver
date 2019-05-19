#include "Request.hpp"

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
    for (int i = 0; i < lines.size(); i++)
    {
        int colonIndex = lines[i].find_first_of(':');
        if(colonIndex == std::string::npos) continue;
        std::string header = lines[i].substr(0,colonIndex);
        std::string value = lines[i].substr(colonIndex,lines[i].length());
        headers[header]=value;
    }
}

std::string Request::getHeader(std::string const & header)
{
    std::string a = headers[header];
    return a;
}

std::string Request::getMethod()
{
    return method;
}
