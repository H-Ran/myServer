#pragma once
#include "common.h"
#include "disposable.h"
#include "thread_mgr.h"

template <class APPClass> inline int MainTemplate()
{
    APPClass *pApp = new APPClass();
    pApp->InitApp();
    // pApp->StartAllThread();
    pApp->Run();
    pApp->Dispose();
    delete pApp;
    return 0;
}

class ServerApp : public IDisposable
{
  public:
    ServerApp(APP_TYPE appType);
    ~ServerApp();

    virtual void InitApp() = 0;
    void Dispose() override;

    void Run() const;
    void UpdateTime() const;

    bool AddListenerToThread(std::string ip, int port) const;

    // signal
    static void Signalhandler(int signalValue);

  protected:
    ThreadMgr *_pThreadMgr;
    APP_TYPE _appType;
};
