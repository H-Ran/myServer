#pragma once
#include <memory>

#include "disposable.h"
#include "network.h"
#include "time.h"
#include "object_block.h"

class RecvNetworkBuffer;
class SendNetworkBuffer;
class Packet;

#define PingTime 1000
#define PingDelayTime 10 * 1000

class ConnectObj : virtual public ObjectBlock
{
public:
    ConnectObj(IDynamicObjectPool *pPool);
    virtual ~ConnectObj();

    void Close();
    bool IsClose() const;

    SOCKET GetSocket() const { return _socket; }
    bool HasRecvData() const;
    Packet *GetRecvPacket() const;
    bool Recv() const;

    bool HasSendData() const;
    void SendPacket(Packet *pPacket) const;
    bool Send() const;

    // ObjectPool
    // same as Constructor
    void TakeoutFromPool(Network *pNetWork, SOCKET socket);
    virtual void BackToPool() override;

protected:
    Network *_pNetWork{nullptr};
    SOCKET _socket;
    RecvNetworkBuffer *_recvBuffer{nullptr};
    SendNetworkBuffer *_sendBuffer{nullptr};
    bool _isClose{false};
};
