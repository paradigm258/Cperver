#ifndef REQUEST_H
#define REQUEST_H

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <sstream>

class Request
{
  private:
    std::string sRequest;
    std::map<std::string, std::string> headers;
    std::map<std::string, std::string> parameters;
    std::string url;
    std::string method;

  public:
    Request();
    Request(std::string const &);
    std::string getHeader(std::string const &);
    std::string getMethod();
    std::string getParameter(std::string const&);
};
#endif