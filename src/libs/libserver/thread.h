#pragma once

#include <list>
#include <thread>

#include "sn_object.h"
#include "thread_obj.h"

class Packet;
class ThreadObject;

class ThreadObjectList {
 public:
  void AddObject(ThreadObject* _obj);
  void Update();
  void AddPacketToList(Packet* pPacket);

 protected:
  // 本线程的所有对象
  std::list<ThreadObject*> _objlist;
  std::mutex _obj_lock;
};

class Thread : public ThreadObjectList, public SnObject, public IDisposable {
 public:
  Thread();
  void Start();
  void Stop();
  bool IsRun() const;
  void Dispose() override;

 private:
  bool _isRun;
  std::thread _thread;
};
