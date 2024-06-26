#pragma once

#include <mutex>

#include "common.h"
#include "singleton.h"
#include "thread.h"
#include <map>

class Packet;
class ThreadObject;
class Network;

class ThreadMgr : public Singleton<ThreadMgr>, public ThreadObjectList
{
  public:
    ThreadMgr();
    void StartAllThread();

    // bool IsGameLoop();
    void NewThread();
    bool AddObjToThread(ThreadObject *obj);
    void AddNetworkToThread(APP_TYPE appType, Network *pNetwork);

    // message
    void DispatchPacket(Packet *pPacket);
    void SendPacket(Packet *pPacket);

    void Dispose();
    bool IsStopAll();
    bool IsDisposeAll();

  private:
    Network *GetNetwork(APP_TYPE appType);

  private:
    uint64 _lastThreadSn{0}; // 最后添加的线程序列号

    std::mutex _thread_lock;
    std::map<uint64, Thread *> _threads;

    // NetworkLoactor
    std::mutex _locator_lock;
    std::map<APP_TYPE, Network *> _networkLocator;
};
