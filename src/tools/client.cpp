#include <thread>
#include <iostream>

#include "client.h"

#include "network/packet.h"
#include <sstream>
#include <random>

Client::Client(int msgCount, std::thread::id threadId) : _msgCount(msgCount)
{
    // gen random seed 根据ID生成随机种子
    std::stringstream strStream;
    strStream << threadId;
    std::string idstr = strStream.str();
    std::seed_seq seed1(idstr.begin(), idstr.end());
    std::minstd_rand0 generator(seed1);
}

void Client::DataHandler()
{
    if (_isCompleted)
        return;
    if (!IsCompleted())
        return;

    if (_index < _msgCount)
    {
        // 发送数据
        if (_lastMsg.empty())
        {
            _lastMsg = GenRandom();
            std::cout << "send. size:" << _lastMsg.length() << " msg:" << _lastMsg << std::endl;

            Packet *pPacket = new Packet(1);
            pPacket->AddBuffer(_lastMsg.c_str(), _lastMsg.length());
            SendPacket(pPacket);
            delete pPacket;
        }
    }
}