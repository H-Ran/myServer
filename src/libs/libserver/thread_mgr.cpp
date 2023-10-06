#include "thread_mgr.h"
#include "common.h"
#include <iostream>
#include "network_listen.h"
ThreadMgr::ThreadMgr()
{
}

void ThreadMgr::StartAllThread()
{
    auto iter = _threads.begin();
    while (iter != _threads.end())
    {
        iter->second->Start();
        ++iter;
    }
}

bool ThreadMgr::IsGameLoop()
{
    for (auto iter = _threads.begin(); iter != _threads.end(); iter++)
    {
        if (iter->second->IsRun())
            return true;
    }
    return false;
}

void ThreadMgr::NewThread()
{
    std::lock_guard<std::mutex> guard(_thread_lock);
    auto pThread = new Thread();
    _threads.insert(std::make_pair(pThread->GetSN(), pThread));
}

// 将对象平均分配到各线程
bool ThreadMgr::AddObjToThread(ThreadObject *obj)
{
    std::lock_guard<std::mutex> guard(_thread_lock);
    // 加入前初始化
    if (!obj->Init())
    {
        std::cout << "AddThreadObj Failed. ThreadObject init failed." << std::endl;
        return false;
    }
    auto iter = _threads.begin();
    if (_lastThreadSn > 0)
    {
        iter = _threads.find(_lastThreadSn);
    }
    if (iter == _threads.end())
    {
        // 没有找到，可能没有线程
        std::cout << "AddThreadObj Failed. no thead." << std::endl;
        return false;
    }
    // 取下一个活动线程
    do
    {
        ++iter;
        if (iter == _threads.end())
            iter = _threads.begin();
    } while (!(iter->second->IsRun()));
    auto pThread = iter->second;
    pThread->AddObject(obj);
    _lastThreadSn = pThread->GetSN();
    return true;
}

void ThreadMgr::Dispose()
{
    auto iter = _threads.begin();
    while (iter != _threads.end())
    {
        Thread *pThread = iter->second;
        pThread->Dispose();
        delete pThread;
        ++iter;
    }
}

void ThreadMgr::DispatchPacket(Packet *pPacket)
{
    // 主线程
    AddPacketToList(pPacket);

    // 子线程
    std::lock_guard<std::mutex> guard(_thread_lock);
    for (auto iter = _threads.begin(); iter != _threads.end(); ++iter)
    {
        Thread *pThread = iter->second;
        pThread->AddPacketToList(pPacket);
    }
}

void ThreadMgr::SendPacket(Packet *pPacket)
{
    NetworkListen *pLocator = static_cast<NetworkListen *>(GetNetwork(APP_Listen));
    pLocator->SendPacket(pPacket);
}
