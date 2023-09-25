#pragma once

#include <list>
#include "network/network_connector.h"
#include <random>
#include <thread>

class Client : public NetworkConnector
{
public:
    explicit Client(int msgCount, std::thread::id threadId);
    void DataHandler();
    bool IsCompleted() const
    {
        return _isCompleted;
    }

private:
    std::string GenRandom();

private:
    // 消息发送次数
    int _msgCount;

    int _index{0};

    // 是否已经发送完所有消息
    bool _isCompleted{false};

    // 最后一条要发送的消息
    std::string _lastMsg{""};

    std::default_random_engine *_pRandomEngine;
};
