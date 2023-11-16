#pragma once
#include <list>
#include <mutex>

#include "disposable.h"
#include "singleton.h"

#include "object_pool_interface.h"

class DynamicObjectPoolMgr : public Singleton<DynamicObjectPoolMgr>, public IDisposable
{
public:
    // TODO:AddPool
    void AddPool(IDynamicObjectPool *pPool);
    void Update();

    void Dispose() override;

private:
    std::mutex _lock;
    std::list<IDynamicObjectPool *> _pools;
};