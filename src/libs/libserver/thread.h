#pragma once

#include "cache_refresh.h"
#include "cache_swap.h"
#include "sn_object.h"
#include "thread_obj.h"
#include <list>
#include <thread>

class Packet;
class ThreadObject;

enum class ThreadState
{
    Init,
    Run,
    Stop
};

class ThreadObjectList : public IDisposable
{
  public:
    void AddObject(ThreadObject *_obj);
    void Update();
    void AddPacketToList(Packet *pPacket);
    void Dispose() override;

  protected:
    //   // 本线程的所有对象
    //   std::list<ThreadObject*> _objlist;
    //   std::mutex _obj_lock;

    // 本线程的所有对象
    std::mutex _obj_lock;
    CacheRefresh<ThreadObject> _objlist;

    // 本线程中的所有待处理包
    std::mutex _packet_lock;
    CacheSwap<Packet> _cachePackets;
};

class Thread : public ThreadObjectList, public SnObject
{
  public:
    Thread();
    void Start();
    // void Stop();
    bool IsStop() const;
    bool IsRun() const;
    bool IsDispose();
    // void Dispose() override;

  private:
    //   bool _isRun;
    ThreadState _state; 
    std::thread _thread;
};
