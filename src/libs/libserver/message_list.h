#pragma once
#include <algorithm>
#include <functional>
#include <list>
#include <map>
#include <mutex>

#include "common.h"
#include "packet.h"

class Packet;

typedef std::function<void(Packet *)> HandleFunction;

class MessageCallBackFunctionInfo;

/// @brief 消息队列基类
/// MessageCallBackFunctionInfo消息回调函数信息类保 存信息
class MessageList : public IDisposable
{
  public:
    void Dispose();
    void AttachCallBackHandler(MessageCallBackFunctionInfo *);
    bool IsFollowMsgId(Packet *packet) const;
    void ProcessPacket(Packet *packet) const;
    // void AddPacket(Packet *pPacket) const;
    static void DispatchPacket(Packet *pPacket);
    static void SendPacket(Packet *pPacket);

  protected:
    MessageCallBackFunctionInfo *_pCallBackFuns{nullptr};
};

/// @brief 消息回调函数信息类
/// 1. 消息packet队列
/// 2.作为回调函数的基类有不同的IsFollowMsgId
class MessageCallBackFunctionInfo
{
  public:
    virtual ~MessageCallBackFunctionInfo() = default;
    virtual bool IsFollowMsgId(Packet *packet) = 0;
    virtual void ProcessPacket(Packet *packet) = 0;
    // void AddPacket(Packet *pPacket);

    //   protected:
    //     std::mutex _msgMutex;
    //     std::list<Packet *> _msgList;
};

/// @brief 无过滤消息回调函数
class MessageCallBackFunction : public MessageCallBackFunctionInfo
{
  public:
    using HandleFunction = std::function<void(Packet *)>;
    void RegisterFunction(int msgId, HandleFunction function);
    bool IsFollowMsgId(Packet *packet) override;
    void ProcessPacket(Packet *packet) override;

    std::map<int, HandleFunction> &GetCallBackHandler()
    {
        return _callbackHandle;
    }

  protected:
    std::map<int, HandleFunction> _callbackHandle;
};

// 有过滤消息回调函数
// 通过指定GetPacketObject函数可以过滤不需要follow的msg
template <class T> class MessageCallBackFunctionFilterObj : public MessageCallBackFunction
{
  public:
    using HandleFunctionWithObj = std::function<void(T *, Packet *)>;
    using HandleGetObject = std::function<T *(SOCKET)>;

    void RegisterFunctionWithObj(int msgId, HandleFunctionWithObj function);
    bool IsFollowMsgId(Packet *packet) override;
    void ProcessPacket(Packet *packet) override;

    // 能否得到Obj的handle函数
    HandleGetObject GetPacketObject{nullptr};

    void CopyFrom(MessageCallBackFunction *pInfo);

  private:
    std::map<int, HandleFunctionWithObj> _callbackHandleWithObj;
};

template <class T>
void MessageCallBackFunctionFilterObj<T>::RegisterFunctionWithObj(int msgId, HandleFunctionWithObj function)
{
    _callbackHandleWithObj[msgId] = function;
}

template <class T> bool MessageCallBackFunctionFilterObj<T>::IsFollowMsgId(Packet *packet)
{
    // 先判断是否有无过滤的handle
    if (MessageCallBackFunction::IsFollowMsgId(packet))
        return true;

    // 再判断是否有有过滤的handle
    if (_callbackHandleWithObj.find(packet->GetMsgId()) != _callbackHandleWithObj.end())
    {
        if (GetPacketObject != nullptr)
        {
            T *pObj = GetPacketObject(packet->GetSocket());
            if (pObj != nullptr)
                return true;
        }
    }
    return false;
}

/// @brief 处理队列里的packet
/// 如果是关心的msgId,则判断是否在MessageCallBackFunction有注册的handle
/// 再判断是否在MessageCallBackFunctionFilterObj中注册了handle和GetPacketObject判断是否属于应该关心的
/// @tparam T
template <class T> void MessageCallBackFunctionFilterObj<T>::ProcessPacket(Packet *packet)
{
    // std::list<Packet *> tmpList;
    // _msgMutex.lock();
    // std::copy(_msgList.begin(), _msgList.end(), std::back_inserter(tmpList));
    // _msgList.clear();
    // _msgMutex.unlock();

    // for (auto packet : tmpList)
    // {
    //     const auto handleIter = _callbackHandle.find(packet->GetMsgId());
    //     if (handleIter != _callbackHandle.end())
    //     {
    //         handleIter->second(packet);
    //         continue;
    //     }

    //     auto handleWithObjIter = _callbackHandleWithObj.find(packet->GetMsgId());
    //     if (handleWithObjIter != _callbackHandleWithObj.end())
    //     {
    //         if (GetPacketObject != nullptr)
    //         {
    //             T *pObj = GetPacketObject(packet->GetSocket());
    //             if (pObj != nullptr)
    //             {
    //                 handleWithObjIter->second(pObj, packet);
    //             }
    //         }
    //     }
    // }
    // tmpList.clear();
    const auto handleIter = _callbackHandle.find(packet->GetMsgId());
    if (handleIter != _callbackHandle.end())
    {
        handleIter->second(packet);
        return;
    }

    auto iter = _callbackHandleWithObj.find(packet->GetMsgId());
    if (iter != _callbackHandleWithObj.end())
    {
        if (GetPacketObject != nullptr)
        {
            T *pObj = GetPacketObject(packet->GetSocket());
            if (pObj != nullptr)
            {
                iter->second(pObj, packet);
            }
        }
    }
}

// template <class T> void MessageCallBackFunctionFilterObj<T>::CopyFrom(MessageCallBackFunction *pInfo)
// {
//     auto copyFromData = pInfo->GetCallBackHandler();
//     std::transform(copyFromData.begin(), copyFromData.end(), std::back_inserter(_callbackHandle),
//                    [](const std::pair<int, HandleFunction> &p) { return p; });
// }
