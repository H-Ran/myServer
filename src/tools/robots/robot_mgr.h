#pragma once

#include "libserver/time.h"
#include "libserver/robot_state_type.h"
#include "libserver/network_connector.h"

class RobotMgr : public NetworkConnector
{
public:
    void RegisterMsgFunction() override;
    bool Init() override;
    void Update() override;

    void ShowInfo();

private:
    // 处理消息MI_RobotSyncState
    void HandleRobotState(Packet *pPacket);
    void NofityServer(RobotStateType maxType);

private:
    bool _isChange{false};
    // <account, RobotStateType>
    std::map<std::string, RobotStateType> _robots;

    timeutil::Time _nextShowInfoTime{0};
};
