#ifndef MYPAGE_HPP
#define MYPAGE_HPP

#include "CperverPage.hpp"

class MyPage : public CperverPage
{
public:
    ~MyPage();
    void handleRequest(Request &, Response &);
};

#endif