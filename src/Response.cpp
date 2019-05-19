#include "Response.hpp"

Response::Response()
{
    status += HTTP_VER" ";
    status += HTTP_STATUS_OK;
}

std::string Response::getRawResponse() const
{
    std::string rResponse = "HTTP/1.1 200 OK\n"
                        "Date: Thu, 19 Feb 2009 12:27:04 GMT\n"
                        "Server: Apache/2.2.3\n"
                        "Last-Modified: Wed, 18 Jun 2003 16:05:58 GMT\n"
                        "ETag: \"56d-9989200-1132c580\"\n"
                        "Content-Type: text/html\n"
                        "Content-Length: 15\n"
                        "Accept-Ranges: bytes\n"
                        "Connection: close\n"
                        "\n"
                        "sdfkjsdnbfkjbsf";
    return rResponse;
}
