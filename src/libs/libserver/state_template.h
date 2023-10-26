#pragma once

#include <map>
#include "robot_state_type.h"

/// 宏定义
/// 1.创建当前状态类实例
/// 2.返回自己的状态枚举
#define DynamicStateCreate(classname, enumType)          \
    static void *CreateState() { return new classname; } \
    RobotStateType GetState() override { return enumType; }

#define DynamicStateBind(classname)\ 
    reinterpret_cast<CreateIstancePt>(&(classname::CreateState))

///@brief 状态模板类,依赖注入状态管理类(StateMgr)T
template <typename enumType, class T>
class StateTemplate
{
public:
    StateTemplate()
    {
    }

    // 注入管理类依赖
    void SetParentObj(T *pObj)
    {
        _pParentObj = pObj;
    }

    virtual ~StateTemplate() {}
    virtual enumType GetState() = 0;
    virtual enumType Update() = 0;

    virtual void EnterState() = 0;
    virtual void LeaveState() = 0;

protected:
    T *_pParentObj;
};

/// @brief 状态管理类
/// @tparam enumType 状态枚举
/// @tparam StateClass 状态类
/// @tparam T 继承状态管理类的类自身
template <typename enumType, class StateClass, class T>
class StateTemplateMgr
{
public:
    virtual ~StateTemplateMgr()
    {
        if (_pState != nullptr)
        {
            delete _pState;
        }
    }

    /// @brief 初始化状态管理类的默认状态(必须选择一个)
    /// @param defaultState 默认状态
    void InitStateTemplateMgr(enumType defaultState)
    {
        _defaultState = defaultState;
        RegisterState();
    }

    /// @brief 改变当前状态
    /// @param stateType 要改变的状态
    void ChangeState(enumType stateType)
    {
        // 从map中提取对应stateType的状态类StateClass
        StateClass *pNewState = CreateStateObj(stateType);
        if (pNewState == nullptr)
        {
            return;
        }
        if (pNewState != nullptr)
        {
            if (_pState != nullptr)
            {
                _pState->LeaveState();
                delete _pState;
            }

            _pState = pNewState;
            _pState->EnterState();
        }
    }

    void UpdateState()
    {
        if (_pState == nullptr)
        {
            ChangeState(_defaultState);
        }
        enumType curState = _pState->Update();
        if (curState != _pState->GetState())
        {
            ChangeState(curState);
        }
    }

protected:
    virtual void RegisterState() = 0;

    ///////////////////////////////////////////////////////////////////////////////////
public:
    typedef StateClass *(*CreateIstancePt)();

    StateClass *CreateStateObj(enumType enumValue)
    {
        auto iter = _dynCreateMap.find(enumValue);
        if (iter == _dynCreateMap.end())
            return nullptr;

        CreateIstancePt np = iter->second;
        StateClass *pState = np();
        pState->SetParentObj(static_cast<T *>(this));
        return pState;
    }

    void RegisterStateClass(enumType enumValue, CreateIstancePt np)
    {
        _dynCreateMap[enumValue] = np;
    }

    ///////////////////////////////////////////////////////////////////////////////////

protected:
    std::map<enumType, CreateIstancePt> _dynCreateMap;
    StateClass *_pState{nullptr};
    enumType _defaultState;
};