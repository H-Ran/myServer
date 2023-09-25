#include "thread.h"

bool Thread::Strat()
{
    _isRun = true;
    _thread = std::thread([this]()
                          {
        while(_isRun){
            Update();
        } });

    return true;
}

void Thread::Stop()
{
    if (IsRun)
    {
        _isRun = false;
    }
}

void Thread::Dispose()
{
    Stop();

    if (_thread.joinable())
        _thread.join();
}

void Thread::Update() {}