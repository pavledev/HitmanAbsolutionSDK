#pragma once

#include <type_traits>

enum class ECallingConvention
{
    Cdecl,
    Stdcall,
    Fastcall,
    Thiscall
};

template<class T, ECallingConvention Convention = ECallingConvention::Cdecl> class EngineFunction;

template<class ReturnType, class... Args, ECallingConvention Convention> class EngineFunction<ReturnType(Args...), Convention>
{
  protected:
    explicit EngineFunction(void* p_Address) : m_Address(p_Address) {}

  public:
    ReturnType Call(Args... p_Args)
    {
        if (m_Address == nullptr)
        {
            if constexpr (std::is_void_v<ReturnType>)
            {
                return;
            }
            else if constexpr (std::is_pointer_v<ReturnType>)
            {
                return nullptr;
            }
            else
            {
                return ReturnType{};
            }
        }

#if defined(_M_IX86)
        if constexpr (Convention == ECallingConvention::Cdecl)
        {
            using FunctionType = ReturnType(__cdecl*)(Args...);
            return CallInternal<FunctionType>(p_Args...);
        }
        else if constexpr (Convention == ECallingConvention::Stdcall)
        {
            using FunctionType = ReturnType(__stdcall*)(Args...);
            return CallInternal<FunctionType>(p_Args...);
        }
        else if constexpr (Convention == ECallingConvention::Fastcall)
        {
            using FunctionType = ReturnType(__fastcall*)(Args...);
            return CallInternal<FunctionType>(p_Args...);
        }
        else if constexpr (Convention == ECallingConvention::Thiscall)
        {
            using FunctionType = ReturnType(__thiscall*)(Args...);
            return CallInternal<FunctionType>(p_Args...);
        }
#else
        // Windows x64 uses a single calling convention.
        using FunctionType = ReturnType (*)(Args...);
        return CallInternal<FunctionType>(p_Args...);
#endif
    }

    [[nodiscard]] bool Exists() const
    {
        return m_Address != nullptr;
    }

  protected:
    template<class FunctionType> ReturnType CallInternal(Args... p_Args)
    {
        if constexpr (std::is_void_v<ReturnType>)
        {
            reinterpret_cast<FunctionType>(m_Address)(p_Args...);
        }
        else
        {
            return reinterpret_cast<FunctionType>(m_Address)(p_Args...);
        }
    }

    void* m_Address = nullptr;
};

template<class T> using CdeclEngineFunction = EngineFunction<T, ECallingConvention::Cdecl>;

template<class T> using StdcallEngineFunction = EngineFunction<T, ECallingConvention::Stdcall>;

template<class T> using FastcallEngineFunction = EngineFunction<T, ECallingConvention::Fastcall>;

template<class T> using ThiscallEngineFunction = EngineFunction<T, ECallingConvention::Thiscall>;
