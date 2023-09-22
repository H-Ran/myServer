
#include "server.h"
#include "network/packet.h"
#include "network/connect_obj.h"
#include <iostream>

bool Server::DataHandler()
{
    for (auto iter = _connects.begin(); iter != _connects.end(); ++iter)
    {
        ConnectObj *pConnectObj = iter->second;
        HandlerOne(pConnectObj);
    }
    if (_isShow)
    {
        _isShow = false;
        std::cout << "accept:" << _acceptCount << "\trecv msg: " << _recvMsgCount << "\tsend msg:" << _sendMsgCount << std::endl;
    }
    return true;
}

void Server::HandlerOne(ConnectObj *pConnectObj)
{
    // 该Server收到Client消息后立马原样发送
    while (pConnectObj->HasRecvData())
    {
        Packet *pPacket = pConnectObj->GetRecvPacket();
        if (pPacket == nullptr)
        {
            break;
        }
        std::string msg(pPacket->GetBuffer(), pPacket->GetDataLength());
        std::cout << "recv msg: " << msg << std::endl;

        pConnectObj->SendPacket(pPacket);

        ++_recvMsgCount;
        ++_sendMsgCount;
        _isShow = true;
    }
}

int Server::Accept()
{
    int rs = NetworkListen::Accept();
    _acceptCount += rs;
    _isShow = true;
    return rs;
}