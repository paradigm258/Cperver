#ifndef CPERERVERPAGE_H
#define CPERERVERPAGE_H

#include "Request.hpp"
#include "Response.hpp"

class CperverPage
{
public:
    virtual ~CperverPage()=0;
    virtual void handleRequest(Request &, Response &)=0;
};

#endif