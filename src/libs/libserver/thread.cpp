#include "thread.h"

#include <iostream>
#include <iterator>

#include "global.h"
#include "packet.h"

void ThreadObjectList::AddObject(ThreadObject *obj)
{
    std::cout << "a Object added" << std::endl;
    std::lock_guard<std::mutex> guard(_obj_lock);

    // 在加入之前初始化一下
    if (!obj->Init())
    {
        std::cout << "AddObject Failed. ThreadObject init failed." << std::endl;
    }
    else
    {
        obj->RegisterMsgFunction();
        // _objlist.push_back(obj);
        _objlist.GetAddCache()->emplace_back(obj);
        const auto pThread = dynamic_cast<Thread *>(this);
        if (pThread != nullptr)
            obj->SetThread(pThread);
    }
}

void ThreadObjectList::Update()
{
    //   std::list<ThreadObject*> _tmpObjs;
    //   _obj_lock.lock();
    //   std::copy(_objlist.begin(), _objlist.end(),
    //   std::back_inserter(_tmpObjs)); _obj_lock.unlock();

    //   for (ThreadObject* pTObj : _tmpObjs) {
    //     pTObj->ProcessPacket();
    //     pTObj->Update();

    //     // 非激活状态，删除
    //     if (!pTObj->IsActive()) {
    //       _obj_lock.lock();
    //       _objlist.remove(pTObj);
    //       _obj_lock.unlock();

    //       pTObj->Dispose();
    //       delete pTObj;
    //     }
    //   }

    //   std::this_thread::sleep_for(std::chrono::milliseconds(1));

    _obj_lock.lock();
    if (_objlist.CanSwap())
    {
        auto pDelList = _objlist.Swap();
        for (auto pOne : pDelList)
        {
            pOne->Dispose();
            delete pOne;
        }
    }
    _obj_lock.unlock();

    _packet_lock.lock();
    if (_cachePackets.CanSwap())
    {
        _cachePackets.Swap();
    }
    _packet_lock.unlock();

    auto pList = _objlist.GetReaderCache();
    auto pMsgList = _cachePackets.GetReaderCache();

    for (auto iter = pList->begin(); iter != pList->end(); ++iter)
    {
        auto pObj = (*iter);
        for (auto itMsg = pMsgList->begin(); itMsg != pMsgList->end(); ++itMsg)
        {
            auto pPacket = (*itMsg);
            if (pObj->IsFollowMsgId(pPacket))
                pObj->ProcessPacket(pPacket);
        }

        pObj->Update();

        if (!pObj->IsActive())
        {
            _objlist.GetRemoveCache()->emplace_back(pObj);
        }
    }

    pMsgList->clear();
}

void ThreadObjectList::AddPacketToList(Packet *pPacket)
{
    std::lock_guard<std::mutex> guard(_obj_lock);
    // for (auto iter = _objlist.begin(); iter != _objlist.end(); ++iter)
    // {
    //     ThreadObject *pObj = *iter;
    //     if (pObj->IsFollowMsgId(pPacket))
    //     {
    //         pObj->AddPacket(pPacket);
    //     }
    // }
    _cachePackets.GetWriterCache()->emplace_back(pPacket);
}
void ThreadObjectList::Dispose()
{
    _objlist.Dispose();
    _cachePackets.Dispose();
}

Thread::Thread()
{
    // this->_isRun = true;
    _state = ThreadState::Init;
    std::cout << "a thread Init" << std::endl;
}

// void Thread::Stop()
// {
//     if (!_isRun)
//     {
//         _isRun = false;
//         if (_thread.joinable())
//             _thread.join();
//     }
// }

void Thread::Start()
{
    std::cout << "Thread::Start." << std::endl;
    _thread = std::thread([&]() {
        _state = ThreadState::Run;
        while (_state == ThreadState::Run)
        {
            Update();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        _state = ThreadState::Stop;
    });
}

bool Thread::IsRun() const
{
    return _state == ThreadState::Run;
}

bool Thread::IsStop() const
{
    return _state == ThreadState::Stop;
}

// void Thread::Dispose()
// {
//     std::list<ThreadObject *>::iterator iter = _objlist.begin();
//     while (iter != _objlist.end())
//     {
//         (*iter)->Dispose();
//         delete (*iter);
//         iter = _objlist.erase(iter);
//     }

//     _objlist.clear();
// }
bool Thread::IsDispose()
{
    if (_thread.joinable())
    {
        const auto theadId = _thread.get_id();
        _thread.join();
        std::cout << "close thread [2/2]. thread sn:" << this->GetSN() << " thread id:" << theadId << std::endl;
        return true;
    }

    return false;
}
