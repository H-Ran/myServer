#include "network.h"
#include "connect_obj.h"
#include <iostream>

#ifdef EPOLL
#else
void Network::Select()
{
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_ZERO(&exceptfds);

    FD_SET(_masterSocket, &readfds);
    FD_SET(_masterSocket, &writefds);
    FD_SET(_masterSocket, &exceptfds);

    // 保存最大的fd
    SOCKET fdmax = _masterSocket;

    // 遍历当前所有连接，将所有有效的Socket加入3个监听集合
    for (auto iter = _connects.begin(); iter != _connects.end(); ++iter)
    {
        if (iter->first > fdmax)
        {
            fdmax = iter->first;
        }
        FD_SET(iter->first, &readfds);
        FD_SET(iter->first, &exceptfds);

        // 判断socket对应的连接有没有要发送的数据
        // 如果有加入写监听集合
        if (iter->second->HasSendData())
        {
            FD_SET(iter->first, &writefds);
        }
        else
        {
            if (_masterSocket == iter->first)
            {
                FD_CLR(_masterSocket, &writefds);
            }
        }
    }

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 50 * 1000;
    int nfds = select(fdmax + 1, &readfds, &writefds, &exceptfds, &timeout);
    if (nfds <= 0)
        return;

    auto iter = _connects.begin();
    while (iter != _connects.end())
    {
        // 检查是否出错
        if (FD_ISSET(iter->first, &exceptfds))
        {
            std::cout << "socket except!! socket:" << iter->first << std::endl;

            iter->second->Dispose();
            delete iter->second;
            iter = _connects.erase(iter);
            continue;
        }
        // 检查是否可读
        if (FD_ISSET(iter->first, &readfds))
        {
            if (!iter->second->Recv())
            {
                iter->second->Dispose();
                delete iter->second;
                iter = _connects.erase(iter);
                continue;
            }
        }
        // 检测是否可以发送数据
        if (FD_ISSET(iter->first, &writefds))
        {
            if (!iter->second->Send())
            {
                iter->second->Dispose();
                delete iter->second;
                iter = _connects.erase(iter);
                continue;
            }
        }

        ++iter;
    }
}

#endif

void Network::Dispose()
{
    auto iter = _connects.begin();
    while (iter != _connects.end())
    {
        iter->second->Dispose();
        delete iter->second;
        ++iter;
    }
    _connects.clear();

#ifdef EPOLL
    ::close(_epfd);
#endif

    // std::cout << "network dispose. close socket:" << _socket << std::endl;
    _sock_close(_masterSocket);
    _masterSocket = -1;
}

#ifndef WIN32
#define SetsockOptType void *
#else
#define SetsockOptType const char *
#endif

void Network::SetSocketOpt(SOCKET socket)
{
    // 1.端口复用
    bool isReuseaddr = true;
    setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (SetsockOptType)&isReuseaddr, sizeof(isReuseaddr));

    // 2.发送、接受的timeout
    int netTimeout = 3000; // 3000ms
    setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, (SetsockOptType)&netTimeout, sizeof(netTimeout));
    setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (SetsockOptType)&netTimeout, sizeof(netTimeout));

#ifndef WIN32

    int keepAlive = 1;
    socklen_t optlen = sizeof(keepAlive);

    int keepIdle = 60 * 2 * 10; // 在socket 没有交互后 多久 开始发送侦测包
    int keepInterval = 30;      // 多次发送侦测包之间的间隔
    int keepCount = 5;          // 侦测包个数

    setsockopt(socket, SOL_SOCKET, SO_KEEPALIVE, (SetsockOptType)&keepAlive, optlen);
    if (getsockopt(socket, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, &optlen) < 0)
    {
        std::cout << "getsockopt SO_KEEPALIVE failed." << std::endl;
    }
    setsockopt(socket, SOL_TCP, TCP_KEEPINTVL, (void *)&keepInterval, optlen);
    setsockopt(socket, SOL_TCP, TCP_KEEPCNT, (void *)&keepCount, optlen);
#else
    int keepAlive = 1;
    socklen_t optlen = sizeof(keepAlive);

    setsockopt(socket, SOL_SOCKET, SO_KEEPALIVE, (SetsockOptType)&keepAlive, optlen);

    DWORD cbBytesReturned;
    tcp_keepalive klive;
    // 启用保活
    klive.onoff = 1;
    klive.keepalivetime = 1200;
    // 重试间隔
    klive.keepaliveinterval = 30;
    WSAIoctl(socket, SIO_KEEPALIVE_VALS, &klive, sizeof(tcp_keepalive), NULL, 0, &cbBytesReturned, NULL, NULL);

#endif

    // 3.非阻塞
    _sock_nonblock(socket);
}

SOCKET Network::CreateSocket()
{
    _sock_init();
    SOCKET socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket == INVALID_SOCKET)
    {
        std::cout << "::socket failed. err:" << _sock_err() << std::endl;
        return socket;
    }
}

void Network::CreateConnectObj(SOCKET socket)
{
    ConnectObj *pConnectObj = new ConnectObj(this, socket);
    _connects.insert(std::make_pair(socket, pConnectObj));
#ifdef EPOLL
    AddEvent(_epfd, socket, EPOLLIN | EPOLLRDHUP);
#endif
}