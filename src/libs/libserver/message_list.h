#pragma once

#include <mutex>
#include <map>
#include <list>
#include <functional>

class Packet;
// void function (Packet*)
typedef std::function<void(Packet *)> HandleFunction;

class MessageList
{
public:
    // 注册协议号，指定一个处理函数
    void RegisterFunction(int msgId, HandleFunction function);
    // 判断是否是关心的协议号，是的话用AddPacket将Packet加入自己的缓存
    bool IsFollowMsgId(int msgId);
    // 在Update中调用ProcessPacket处理缓存的Packet
    void ProcessPacket();
    void AddPacket(Packet *pPacket);
    static void DispatchPacket(Packet *pPacket);
    static void SendPacket(Packet *pPacket);

protected:
    std::mutex _msgMutex;
    std::list<Packet *> _msgList;
    std::map<int, HandleFunction> _callbackHandle;
};