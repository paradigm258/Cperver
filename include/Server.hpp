#ifndef SERVER_H
#define SERVER_H
#include <WS2tcpip.h>
#include <WinSock2.h>
#include <Windows.h>

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <queue>
#include <thread>
#include <future>

#include <time.h>
#include <stdio.h>

#include "Response.hpp"
#include "Request.hpp"
#include "HttpUtil.hpp"

#define DEFAULT_PORT "20715"
#define BUFLEN 1024
#define POOLSIZE 2
#define INIT_SUCCESS 0
#define MAXTIMEOUT 2000 //ms

class Server
{

private:
  WSADATA wsaData;
  SOCKET ListenSocket;
  bool running;
  timeval timeOutDuration;
  fd_set fds;
  std::mutex mtx;
  std::condition_variable cv;
  std::queue<SOCKET> tasks;
  void Error(std::string const &, int);
  Request recvRequest(SOCKET);
  bool sendResponse(SOCKET, Response const &);
  bool checkEndRequest(std::string const &);
  void ClearScreen();
  void serverCallback();
  void threadCallback();
  bool ready(SOCKET);
  void cleanup();
  std::function<void(Request &, Response &)> getRequestCallbackHandler(Request &);

public:
  Server();
  int Init();
  int Start();
};
#endif
