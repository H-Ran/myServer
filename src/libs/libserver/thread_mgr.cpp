#include "thread_mgr.h"
#include "common.h"
#include <iostream>

#include "network.h"
#include "network_listen.h"

ThreadMgr::ThreadMgr()
{
}

void ThreadMgr::StartAllThread()
{
    auto iter = _threads.begin();
    std::cout << "ThreadMgr::StartAllThread." << std::endl;
    while (iter != _threads.end())
    {
        iter->second->Start();
        ++iter;
    }
}

bool ThreadMgr::IsGameLoop()
{
    for (auto iter = _threads.begin(); iter != _threads.end(); ++iter)
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

// 平均加入各线程中
bool ThreadMgr::AddObjToThread(ThreadObject *obj)
{
    std::lock_guard<std::mutex> guard(_thread_lock);
    std::cout << "AddObject adding..." << std::endl;
    // 找到上一次的线程
    auto iter = _threads.begin();
    if (_lastThreadSn > 0)
    {
        iter = _threads.find(_lastThreadSn);
    }

    if (iter == _threads.end())
    {
        // 没有找到，可能没有配线程
        std::cout << "AddThreadObj Failed. no thead." << std::endl;
        return false;
    }

    // BUG:一直取不到下一个线程
    //  取到它的下一个活动线程
    do
    {
        ++iter;
        if (iter == _threads.end())
            iter = _threads.begin();
        // std::cout << "Get Next Running Thread : " << iter->second->IsRun() << std::endl;
    } while (!(iter->second->IsRun()));
    std::cout << "Got Next Running Thread Successful." << std::endl;

    std::cout << "AddObject successful." << std::endl;
    auto pThread = iter->second;
    pThread->AddObject(obj);
    _lastThreadSn = pThread->GetSN();
    // std::cout << "add obj to thread.id:" << pThread->GetSN() << std::endl;

    return true;
}

void ThreadMgr::AddNetworkToThread(APP_TYPE appType, Network *pNetwork)
{
    if (!AddObjToThread(pNetwork))
        return;

    std::lock_guard<std::mutex> guard(_locator_lock);
    _networkLocator[appType] = pNetwork;
}

Network *ThreadMgr::GetNetwork(APP_TYPE appType)
{
    std::lock_guard<std::mutex> guard(_locator_lock);

    auto iter = _networkLocator.find(appType);
    if (iter == _networkLocator.end())
        return nullptr;

    return iter->second;
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
