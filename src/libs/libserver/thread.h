#pragma once

#include <thread>
#include <list>

#include "thread_obj.h"
#include "sn_object.h"

class Thread : public IDisposable, SnObject
{
public:
    Thread();
    void AddThreadObj(ThreadObject *_obj);

    void Start();
    void Stop();
    void Update();
    bool IsRun() const;
    void Dispose() override;

private:
    // 本线程的所有对象
    std::list<ThreadObject *> _objlist;
    // 将这一帧要更新的对象加锁放入tmp,加入后解锁。便可以对objlist进行添加对象等操作
    // 不因为锁占用资源
    std::list<ThreadObject *> _tmpObjs;
    std::mutex _thread_lock;

    bool _isRun;
    std::thread _thread;
};
