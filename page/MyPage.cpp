#include "MyPage.hpp"

#include <fstream>
#include <iostream>

MyPage::~MyPage()
{
}

void MyPage::handleRequest(Request &req, Response &res)
{
    auto &a = res.getContentStream();
    std::ifstream f;
    f.open("a.html");
    a << f.rdbuf();
    res.setHeader("Content-Length", std::to_string(a.tellp()));
}

extern "C" MyPage *CreateCperverPage()
{
    return new MyPage();
}