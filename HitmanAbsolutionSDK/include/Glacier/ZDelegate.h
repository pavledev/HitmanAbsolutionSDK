#pragma once

template <typename T>
class ZDelegate;

template <typename ReturnType, typename... Args>
class alignas(8) ZDelegate<ReturnType(Args...)>
{
public:
	using MemberCallback_t = ReturnType(*)(void*, Args...);

	ZDelegate() :
		m_mfp(nullptr),
		pInst(nullptr)
	{
	}

	ZDelegate(MemberCallback_t memberCallback, void* instance) :
		m_mfp(memberCallback),
		pInst(instance)
	{
	}

    ReturnType operator()(Args... p_Args)
    {
        if (pInst)
        {
            return reinterpret_cast<ReturnType(__thiscall*)(void*, Args...)>(m_mfp)(pInst, p_Args...);
        }

        return reinterpret_cast<ReturnType(__cdecl*)(Args...)>(m_mfp)(p_Args...);
    }

	MemberCallback_t m_mfp;
	void* pInst;
};

template <typename InstanceType, typename T>
class ZMemberDelegate;

template <typename InstanceType, typename ReturnType, typename... Args>
class ZMemberDelegate<InstanceType, ReturnType(Args...)> : public ZDelegate<ReturnType(Args...)>
{
private:
    typedef ReturnType(__thiscall InstanceType::* MemberFunction_t)(Args...);

    union MemberFunctionCaster
    {
        MemberFunction_t memberFunction;
        typename ZDelegate<ReturnType(Args...)>::MemberCallback_t memberCallback;
    };

public:
    ZMemberDelegate(InstanceType* instance, MemberFunction_t memberFunction) :
        ZDelegate<ReturnType(Args...)>(GetMemberCb(memberFunction), instance)
    {
    }

private:
    typename ZDelegate<ReturnType(Args...)>::MemberCallback_t GetMemberCb(MemberFunction_t memberFunction)
    {
        MemberFunctionCaster s_Caster;
        s_Caster.memberFunction = memberFunction;

        return s_Caster.memberCallback;
    }
};

template <class T>
class ZStaticDelegate;

template <typename ReturnType, typename... Args>
class ZStaticDelegate<ReturnType(Args...)> : public ZDelegate<ReturnType(Args...)>
{
public:
    ZStaticDelegate(typename ZDelegate<ReturnType(Args...)>::StaticCallback_t callback) :
        ZDelegate<ReturnType(Args...)>(callback, nullptr)
    {
    }
};
