#pragma once

#include <cassert>
#include <type_traits>

#include "Common.h"
#include "EngineFunction.h"

class HookBase : public IDestructible
{
  public:
    ~HookBase() override = default;

    virtual void RemoveDetoursWithContext(void* p_Context) = 0;
    virtual void RemoveAllDetours() = 0;

  protected:
    struct Detour
    {
        void* m_Context;
        void* m_DetourFunc;
    };

    virtual void AddDetourInternal(void*, void*) = 0;
    virtual void RemoveDetourInternal(void*) = 0;
    virtual Detour** GetDetours() = 0;
    virtual void LockForCall() = 0;
    virtual void UnlockForCall() = 0;
    virtual void Remove() = 0;

    void* m_OriginalFunc = nullptr;

    friend class HookRegistry;
};

namespace HookAction
{
    struct Return
    {};

    struct Continue
    {};
}

template<typename T> class HookResult
{
  public:
    HookResult(HookAction::Return, T p_Value) : m_ReturnVal(p_Value), m_HasReturnVal(true) {}

    HookResult(HookAction::Continue) : m_HasReturnVal(false) {}

    T m_ReturnVal;
    bool m_HasReturnVal;
};

template<> class HookResult<void>
{
  public:
    HookResult(HookAction::Return) : m_HasReturnVal(true) {}
    HookResult(HookAction::Continue) : m_HasReturnVal(false) {}
    bool m_HasReturnVal;
};

template<typename T, ECallingConvention Convention> struct HookAbi;

template<typename R, class... A> struct HookAbi<R(A...), ECallingConvention::Cdecl>
{
    using Original = R(__cdecl*)(A...);
    using Entry = Original;
};

template<typename R, class... A> struct HookAbi<R(A...), ECallingConvention::Stdcall>
{
    using Original = R(__stdcall*)(A...);
    using Entry = Original;
};

template<typename R, class... A> struct HookAbi<R(A...), ECallingConvention::Fastcall>
{
    using Original = R(__fastcall*)(A...);
    using Entry = Original;
};

template<typename R, class This, class... A> struct HookAbi<R(This, A...), ECallingConvention::Thiscall>
{
    using Original = R(__thiscall*)(This, A...);
    using Entry = R(__fastcall*)(This, void*, A...);
};

template<typename T, ECallingConvention Convention = ECallingConvention::Cdecl> class Hook;

template<typename T> using CdeclHook = Hook<T, ECallingConvention::Cdecl>;
template<typename T> using StdcallHook = Hook<T, ECallingConvention::Stdcall>;
template<typename T> using FastcallHook = Hook<T, ECallingConvention::Fastcall>;
template<typename T> using ThiscallHook = Hook<T, ECallingConvention::Thiscall>;

template<typename R, class... A, ECallingConvention Convention> class Hook<R(A...), Convention> : public HookBase
{
  public:
    using OriginalFunc_t = typename HookAbi<R(A...), Convention>::Original;
    using EntryFunc_t = typename HookAbi<R(A...), Convention>::Entry;
    using DetourFunc_t = HookResult<R>(__cdecl*)(void*, Hook*, A...);

    void AddDetour(void* p_Context, DetourFunc_t p_Detour)
    {
        AddDetourInternal(p_Context, reinterpret_cast<void*>(p_Detour));
    }

    void RemoveDetour(DetourFunc_t p_Detour)
    {
        RemoveDetourInternal(reinterpret_cast<void*>(p_Detour));
    }

    R Call(A... p_Args)
    {
        LockForCall();

        auto detours = GetDetours();

        for (auto detour = *detours; detour != nullptr; detour = *++detours)
        {
            auto fn = reinterpret_cast<DetourFunc_t>(detour->m_DetourFunc);
            auto result = fn(detour->m_Context, this, p_Args...);

            if (result.m_HasReturnVal)
            {
                UnlockForCall();

                if constexpr (std::is_void_v<R>)
                {
                    return;
                }
                else
                {
                    return result.m_ReturnVal;
                }
            }
        }

        UnlockForCall();

        if constexpr (std::is_void_v<R>)
        {
            CallOriginal(p_Args...);
        }
        else
        {
            return CallOriginal(p_Args...);
        }
    }

    R CallOriginal(A... p_Args)
    {
        assert(m_OriginalFunc != nullptr);

        auto original = reinterpret_cast<OriginalFunc_t>(m_OriginalFunc);

        if constexpr (std::is_void_v<R>)
        {
            original(p_Args...);
        }
        else
        {
            return original(p_Args...);
        }
    }
};

#define DECLARE_DETOUR_WITH_CONTEXT_CC(ContextType, Convention, ReturnType, DetourName, ...)                     \
    template<typename... Args> static HookResult<ReturnType> __cdecl DetourName(void* p_Context, Args... p_Args) \
    {                                                                                                            \
        return reinterpret_cast<ContextType*>(p_Context)->DetourName##_Internal(p_Args...);                      \
    }                                                                                                            \
    HookResult<ReturnType> DetourName##_Internal(Hook<ReturnType(__VA_ARGS__), ECallingConvention::Convention>* p_Hook, __VA_ARGS__);

#define DEFINE_DETOUR_WITH_CONTEXT_CC(ContextType, Convention, ReturnType, DetourName, ...) \
    HookResult<ReturnType> ContextType::DetourName##_Internal(Hook<ReturnType(__VA_ARGS__), ECallingConvention::Convention>* p_Hook, __VA_ARGS__)

#define DECLARE_STATIC_DETOUR_CC(Convention, ReturnType, DetourName, ...) \
    static HookResult<ReturnType> __cdecl DetourName(void*, Hook<ReturnType(__VA_ARGS__), ECallingConvention::Convention>* p_Hook, __VA_ARGS__);

#define DEFINE_STATIC_DETOUR_CC(ParentType, Convention, ReturnType, DetourName, ...) \
    HookResult<ReturnType> __cdecl ParentType::DetourName(void*, Hook<ReturnType(__VA_ARGS__), ECallingConvention::Convention>* p_Hook, __VA_ARGS__)

#define DECLARE_CDECL_DETOUR_WITH_CONTEXT(ContextType, ReturnType, DetourName, ...) \
    DECLARE_DETOUR_WITH_CONTEXT_CC(ContextType, Cdecl, ReturnType, DetourName, __VA_ARGS__)

#define DECLARE_STDCALL_DETOUR_WITH_CONTEXT(ContextType, ReturnType, DetourName, ...) \
    DECLARE_DETOUR_WITH_CONTEXT_CC(ContextType, Stdcall, ReturnType, DetourName, __VA_ARGS__)

#define DECLARE_FASTCALL_DETOUR_WITH_CONTEXT(ContextType, ReturnType, DetourName, ...) \
    DECLARE_DETOUR_WITH_CONTEXT_CC(ContextType, Fastcall, ReturnType, DetourName, __VA_ARGS__)

#define DECLARE_THISCALL_DETOUR_WITH_CONTEXT(ContextType, ReturnType, DetourName, ...) \
    DECLARE_DETOUR_WITH_CONTEXT_CC(ContextType, Thiscall, ReturnType, DetourName, __VA_ARGS__)

#define DEFINE_CDECL_DETOUR_WITH_CONTEXT(ContextType, ReturnType, DetourName, ...) \
    DEFINE_DETOUR_WITH_CONTEXT_CC(ContextType, Cdecl, ReturnType, DetourName, __VA_ARGS__)

#define DEFINE_STDCALL_DETOUR_WITH_CONTEXT(ContextType, ReturnType, DetourName, ...) \
    DEFINE_DETOUR_WITH_CONTEXT_CC(ContextType, Stdcall, ReturnType, DetourName, __VA_ARGS__)

#define DEFINE_FASTCALL_DETOUR_WITH_CONTEXT(ContextType, ReturnType, DetourName, ...) \
    DEFINE_DETOUR_WITH_CONTEXT_CC(ContextType, Fastcall, ReturnType, DetourName, __VA_ARGS__)

#define DEFINE_THISCALL_DETOUR_WITH_CONTEXT(ContextType, ReturnType, DetourName, ...) \
    DEFINE_DETOUR_WITH_CONTEXT_CC(ContextType, Thiscall, ReturnType, DetourName, __VA_ARGS__)
