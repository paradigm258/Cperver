#ifndef SERVER_H
#define SERVER_H
#include <WS2tcpip.h>
#include <WinSock2.h>
#include <Windows.h>

#include <map>
#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <time.h>

#include "Response.hpp"
#include "Request.hpp"
#include "CperverPage.hpp"

#define DEFAULT_PORT "20715"
#define BUFLEN 1024
#define POOLSIZE 8
#define INIT_SUCCESS 0
#define MAXTIMEOUT 2000 //ms

typedef CperverPage *(*FCreate)();

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
  void Error(const std::string &, int);
  Request recvRequest(const SOCKET);
  bool sendResponse(const SOCKET, Response &);
  bool checkEndRequest(const std::string &);
  void ClearScreen();
  void serverCallback();
  void handlerThreadCallback();
  bool ready(SOCKET);
  void cleanup();
  CperverPage *getCperverPageHandler(HMODULE, Request &);

public:
  Server();
  int Init();
  int Start();
};
#endif
