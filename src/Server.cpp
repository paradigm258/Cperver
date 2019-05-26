#include <iostream>

#include "Server.hpp"

Server::Server()
{
    ZeroMemory(&wsaData, sizeof(wsaData));
    running = false;
    timeOutDuration.tv_sec = 1;
    timeOutDuration.tv_usec = 0;
    ListenSocket = INVALID_SOCKET;
}

void Server::Error(std::string const &err, int code)
{
    std::cerr << err << " failed: " << code << "\n";
}

int Server::Init()
{
    //Set up winsock
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        Error("WSAStartup", iResult);
        return 1;
    }

    //Set up data for port
    addrinfo *result = nullptr, hint;

    //Set hint as connection option
    ZeroMemory(&hint, sizeof(hint)); //Zero out hint
    hint.ai_family = AF_INET;        //Ipv4
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_protocol = IPPROTO_TCP;
    hint.ai_flags = AI_PASSIVE;

    //Resolve the address and port to be use by the server
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hint, &result);
    if (iResult != 0)
    {
        Error("getaddrinfo", iResult);
        WSACleanup();
        return 1;
    }

    //Create listen socket
    ListenSocket = socket(result->ai_flags, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET)
    {
        Error("socket", WSAGetLastError());
        WSACleanup();
        freeaddrinfo(result);
        return 1;
    }

    //Bind listen socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    freeaddrinfo(result);
    if (iResult == SOCKET_ERROR)
    {
        Error("bind", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    //Start listening on socket
    if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        Error("listen", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    return INIT_SUCCESS;
}

int Server::Start()
{
    running = true;
    std::thread requestHandler(&Server::serverCallback, this);
    const int poolSize = POOLSIZE;

    std::thread threadPool[poolSize];
    for (int i = 0; i < poolSize; i++)
    {
        threadPool[i] = std::thread(&Server::handlerThreadCallback, this);
    }

    while (running)
    {
        std::string a;
        std::getline(std::cin, a);
        if (a == "quit")
        {
            std::lock_guard<std::mutex> l(mtx);
            running = false;
            cv.notify_all();
        }
        if (a == "clear")
        {
            ClearScreen();
        }
    }
    requestHandler.join();
    for (int i = 0; i < poolSize; i++)
    {
        threadPool[i].join();
    }
    cleanup();
    if (WSACleanup() == SOCKET_ERROR)
    {
        Error("WSACleanup", WSAGetLastError());
    }
    return 0;
}

void Server::cleanup()
{
    while (!tasks.empty())
    {
        if (closesocket(tasks.front()) == SOCKET_ERROR)
        {
            Error("closesocket", WSAGetLastError());
        };
        tasks.pop();
    }
}

void Server::ClearScreen()
{
    HANDLE hStdOut;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD count;
    DWORD cellCount;
    COORD homeCoords = {0, 0};

    hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdOut == INVALID_HANDLE_VALUE)
        return;

    /* Get the number of cells in the current buffer */
    if (!GetConsoleScreenBufferInfo(hStdOut, &csbi))
        return;
    cellCount = csbi.dwSize.X * csbi.dwSize.Y;

    /* Fill the entire buffer with spaces */
    if (!FillConsoleOutputCharacter(
            hStdOut,
            (TCHAR)' ',
            cellCount,
            homeCoords,
            &count))
        return;

    /* Fill the entire buffer with the current colors and attributes */
    if (!FillConsoleOutputAttribute(
            hStdOut,
            csbi.wAttributes,
            cellCount,
            homeCoords,
            &count))
        return;

    /* Move the cursor home */
    SetConsoleCursorPosition(hStdOut, homeCoords);
}

void Server::handlerThreadCallback()
{
    while (running)
    {
        //Create lock and lock mutex
        std::unique_lock<std::mutex> l(mtx);
        //Wait for lock then for either a stop signal or an available task
        cv.wait(l, [this]() { return !running || !tasks.empty(); });
        if (!running)
        {
            std::cout << "Done " << std::this_thread::get_id() << std::endl;
            l.unlock();
            continue;
        }
        //Get Client Socket from queue
        SOCKET ClientSocket = std::move(tasks.front());
        tasks.pop();
        //Unlock lock early after obtain client socket
        l.unlock();
        //Try to receive request from client socket and then response to client
        try
        {
            Request request = recvRequest(ClientSocket);
            Response response;
            //Load Page callback from dll
            HMODULE hDLL = NULL;
            CperverPage *fCallback = getCperverPageHandler(hDLL, request);
            if (fCallback != nullptr)
            {
                fCallback->handleRequest(request, response);
                delete fCallback;
            }
            FreeLibrary(hDLL);
            //Send handled response
            sendResponse(ClientSocket, response);
        }
        catch (const char *msg)
        {
            Error(msg, 0);
        }
        closesocket(ClientSocket);
    }
}

void Server::serverCallback()
{
    while (running)
    {
        std::unique_lock<std::mutex> l(mtx, std::defer_lock);
        if (ready(ListenSocket))
        {
            SOCKET ClientSocket = accept(ListenSocket, NULL, NULL);
            if (ClientSocket == INVALID_SOCKET)
            {
                Error("accept", WSAGetLastError());
            }
            else
            {
                l.lock();
                tasks.push(ClientSocket);
                l.unlock();
                cv.notify_all();
            }
        }
    }
    if (closesocket(ListenSocket) == SOCKET_ERROR)
    {
        Error("closesocket", WSAGetLastError());
    };
}
Request Server::recvRequest(const SOCKET ClientSocket)
{
    char buf[BUFLEN] = {0};
    std::string sRequest;
    int iResult = 0;
    bool requestEnded = false;
    do
    {
        if (ready(ClientSocket))
        {
            iResult = recv(ClientSocket, buf, BUFLEN, 0);
            if (iResult == SOCKET_ERROR)
            {
                Error("recv", WSAGetLastError());
                break;
            }
            sRequest.append(buf, iResult);
            requestEnded = checkEndRequest(sRequest);
        }
        else
        {
            Error("Timeout", timeOutDuration.tv_sec);
            break;
        }

    } while (!requestEnded);
    std::cout << "Thread id: " << std::this_thread::get_id() << "\n"
              << "--------Start--------\n"
              << sRequest
              << "---------End---------\n"
              << "Result: " << iResult << "\n"
              << (requestEnded ? "got it" : "timeout")
              << "\n"
              << std::endl;
    if (requestEnded)
    {
        return Request(sRequest);
    }
    else
        throw "Invalid Request";
}
bool Server::sendResponse(const SOCKET ClientSocket, Response &response)
{
    int iResult = 0;
    //Send status line
    iResult = send(ClientSocket, response.getStatusLine().c_str(), response.getStatusLine().size(), 0);
    //Prepare and send headers
    const auto &headers = response.getHeaders();
    for (const auto &header : headers)
    {
        std::string buf = header.first + ": " + header.second + "\n";
        iResult = send(ClientSocket, buf.c_str(), buf.size(), 0);
    }
    //Line seperate between headers and body
    iResult = send(ClientSocket, "\n", 1, 0);
    //Send content stream
    auto &contentStream = response.getContentStream();
    while (contentStream)
    {
        char buff[128] = {0};
        contentStream.read(buff, 128);
        iResult = send(ClientSocket, buff, contentStream.gcount(), 0);
    }
    return iResult > 0;
}
bool Server::checkEndRequest(const std::string &sRequest)
{
    if (sRequest.length() > 4)
    {
        bool isPost = !strcmp(sRequest.substr(0, 4).c_str(), "POST");
        return sRequest.compare(sRequest.size() - 4, 4, isPost ? END_POST : END_GET) == 0;
    }
    else
        return false;
}

bool Server::ready(SOCKET sock)
{
    FD_ZERO(&fds);
    FD_SET(sock, &fds);
    return (select((int)sock + 1, &fds, NULL, NULL, &timeOutDuration) == 1);
}

CperverPage *Server::getCperverPageHandler(HMODULE hDll, Request &req)
{
    hDll = LoadLibrary("MyPage.dll");
    if (!hDll)
    {
        Error("Load dll", GetLastError());
        throw "a";
    }
    FCreate CreateCperverPage = (FCreate)GetProcAddress(hDll, "CreateCperverPage");
    if (!CreateCperverPage)
    {
        Error("Get Page", GetLastError());
        throw "a";
    }
    return CreateCperverPage();
}
