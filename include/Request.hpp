#ifndef REQUEST_H
#define REQUEST_H

#include <string>
#include <map>

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
  virtual std::string getHeader(const std::string &);
  virtual std::string getMethod();
  virtual std::string getParameter(std::string const &);
};
#endif