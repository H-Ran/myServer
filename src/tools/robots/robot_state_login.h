#pragma once

#include "robot_state.h"

/// @brief 正在连接Login
class RobotStateLoginConnecting : public RobotState
{
public:
    DynamicStateCreate(RobotStateLoginConnecting, RobotState_Login_Connecting);

    void OnEnterState() override;
    RobotStateType OnUpdate() override;
};

/// @brief Login连接成功
class RobotStateLoginConnected : public RobotState
{
public:
    DynamicStateCreate(RobotStateLoginConnected, RobotState_Login_Connected);

    void OnEnterState() override;
};

/// @brief 登录成功
class RobotStateLoginLogined : public RobotState
{
public:
    DynamicStateCreate(RobotStateLoginLogined, RobotState_Login_Logined);
};
