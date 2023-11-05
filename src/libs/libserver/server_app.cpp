#include "server_app.h"
#include "common.h"
#include "network_listen.h"
#include <iostream>

ServerApp::ServerApp(APP_TYPE appType)
{
    signal(SIGINT, Signalhandler);

    _appType = appType;

    Global::Instance();
    ThreadMgr::Instance();
    _pThreadMgr = ThreadMgr::GetInstance();
    UpdateTime();

    // 创建线程
    for (int i = 0; i < 3; i++)
    {
        _pThreadMgr->NewThread();
    }
    // 开始所有线程
    _pThreadMgr->StartAllThread();
}

ServerApp::~ServerApp()
{
    _pThreadMgr->DestroyInstance();
}

void ServerApp::Dispose()
{
    _pThreadMgr->Dispose();
}

void ServerApp::Signalhandler(const int signalValue)
{
    switch (signalValue)
    {
#if ENGINE_PLATFORM != PLATFORM_WIN32
    case SIGSTOP:
    case SIGQUIT:
#endif
    case SIGTERM:
    case SIGINT:
        Global::GetInstance()->IsStop = true;
        break;
    }

    std::cout << "\nrecv signal. value:" << signalValue << " Global IsStop::" << Global::GetInstance()->IsStop
              << std::endl;
}

void ServerApp::Run() const
{
    std::cout << "Update ThreadMgr." << std::endl;
    bool isRun = true;
    while (isRun)
    {
        UpdateTime();

        _pThreadMgr->Update();

        isRun = _pThreadMgr->IsGameLoop();
    }
}

void ServerApp::UpdateTime() const
{
    auto timeValue = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
    Global::GetInstance()->TimeTick = timeValue.time_since_epoch().count();

#if ENGINE_PLATFORM != PLATFORM_WIN32
    auto tt = std::chrono::system_clock::to_time_t(timeValue);
    struct tm *ptm = localtime(&tt);
    Global::GetInstance()->YearDay = ptm->tm_yday;
#else
    auto tt = std::chrono::system_clock::to_time_t(timeValue);
    struct tm tm;
    localtime_s(&tm, &tt);
    Global::GetInstance()->YearDay = tm.tm_yday;
#endif
}

bool ServerApp::AddListenerToThread(std::string ip, int port) const
{
    NetworkListen *pListener = new NetworkListen();
    if (!pListener->Listen(ip, port))
    {
        delete pListener;
        return false;
    }

    _pThreadMgr->AddNetworkToThread(APP_Listen, pListener);
    return true;
}
