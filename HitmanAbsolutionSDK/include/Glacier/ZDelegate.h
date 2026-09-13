#pragma once

template<typename T> class ZDelegate;

template<typename ReturnType, typename... Args> class alignas(8) ZDelegate<ReturnType(Args...)>
{
  public:
    using MemberCallback = ReturnType (*)(void*, Args...);

    ZDelegate() : m_mfp(nullptr), pInst(nullptr) {}

    ZDelegate(MemberCallback p_MemberCallback, void* p_Instance) : m_mfp(p_MemberCallback), pInst(p_Instance) {}

    ReturnType operator()(Args... p_Args)
    {
        if (pInst)
        {
            return reinterpret_cast<ReturnType(__thiscall*)(void*, Args...)>(m_mfp)(pInst, p_Args...);
        }

        return reinterpret_cast<ReturnType(__cdecl*)(Args...)>(m_mfp)(p_Args...);
    }

    MemberCallback m_mfp;
    void* pInst;
};

template<typename InstanceType, typename T> class ZMemberDelegate;

template<typename InstanceType, typename ReturnType, typename... Args>
class ZMemberDelegate<InstanceType, ReturnType(Args...)> : public ZDelegate<ReturnType(Args...)>
{
  private:
    using MemberFunction = ReturnType (__thiscall InstanceType::*)(Args...);

    union MemberFunctionCaster
    {
        MemberFunction m_MemberFunction;
        typename ZDelegate<ReturnType(Args...)>::MemberCallback m_MemberCallback;
    };

  public:
    ZMemberDelegate(InstanceType* p_Instance, MemberFunction p_MemberFunction)
        : ZDelegate<ReturnType(Args...)>(GetMemberCb(p_MemberFunction), p_Instance)
    {}

  private:
    typename ZDelegate<ReturnType(Args...)>::MemberCallback GetMemberCb(MemberFunction p_MemberFunction)
    {
        MemberFunctionCaster caster;
        caster.m_MemberFunction = p_MemberFunction;

        return caster.m_MemberCallback;
    }
};

template<class T> class ZStaticDelegate;

template<typename ReturnType, typename... Args> class ZStaticDelegate<ReturnType(Args...)> : public ZDelegate<ReturnType(Args...)>
{
  public:
    ZStaticDelegate(typename ZDelegate<ReturnType(Args...)>::StaticCallback_t callback) : ZDelegate<ReturnType(Args...)>(callback, nullptr) {}
};
