#pragma once

#include <cstdint>
#include <cstddef>

#include <fmt/format.h>

#include "EngineFunction.h"
#include "ModSDK.h"
#include "Utils/ProcessUtils.h"
#include "Logging.h"

namespace Detail
{
    inline uintptr_t SearchEnginePattern(const char* p_Pattern, const char* p_Mask)
    {
        const auto* pattern = reinterpret_cast<const uint8_t*>(p_Pattern);

        return util::ProcessUtils::SearchPattern(ModSDK::GetInstance().GetModuleBase(), ModSDK::GetInstance().GetSizeOfCode(), pattern, p_Mask);
    }

    inline void LogFunctionAddressNotFound(const char* p_FunctionName)
    {
        Logger::Error(
            "Could not locate address for function '{}'. "
            "This probably means that the game was updated "
            "and the SDK requires changes.",
            p_FunctionName
        );
    }

    inline void LogFunctionAddressFound(const char* p_FunctionName, void* p_Address)
    {
        Logger::Debug("Successfully located function '{}' at address {}.", p_FunctionName, fmt::ptr(p_Address));
    }
}

// Locates a function by scanning for a byte pattern at the
// beginning of its prologue.
template<class T, ECallingConvention Convention = ECallingConvention::Cdecl> class PatternEngineFunction;

template<class ReturnType, class... Args, ECallingConvention Convention>
class PatternEngineFunction<ReturnType(Args...), Convention> final : public EngineFunction<ReturnType(Args...), Convention>
{
  public:
    PatternEngineFunction(const char* p_FunctionName, const char* p_Pattern, const char* p_Mask)
        : EngineFunction<ReturnType(Args...), Convention>(GetTarget(p_Pattern, p_Mask))
    {
        if (this->m_Address == nullptr)
        {
            Detail::LogFunctionAddressNotFound(p_FunctionName);
            return;
        }

        Detail::LogFunctionAddressFound(p_FunctionName, this->m_Address);
    }

  private:
    static void* GetTarget(const char* p_Pattern, const char* p_Mask)
    {
        return reinterpret_cast<void*>(Detail::SearchEnginePattern(p_Pattern, p_Mask));
    }
};

// Locates an E8 CALL instruction and resolves its signed
// 32-bit relative displacement.
template<class T, ECallingConvention Convention = ECallingConvention::Cdecl> class PatternRelativeEngineFunction;

template<class ReturnType, class... Args, ECallingConvention Convention>
class PatternRelativeEngineFunction<ReturnType(Args...), Convention> final : public EngineFunction<ReturnType(Args...), Convention>
{
  public:
    PatternRelativeEngineFunction(const char* p_FunctionName, const char* p_Pattern, const char* p_Mask)
        : EngineFunction<ReturnType(Args...), Convention>(GetTarget(p_FunctionName, p_Pattern, p_Mask))
    {
        if (this->m_Address == nullptr)
        {
            Detail::LogFunctionAddressNotFound(p_FunctionName);
            return;
        }

        Detail::LogFunctionAddressFound(p_FunctionName, this->m_Address);
    }

  private:
    static void* GetTarget(const char* p_FunctionName, const char* p_Pattern, const char* p_Mask)
    {
        const uintptr_t target = Detail::SearchEnginePattern(p_Pattern, p_Mask);

        if (target == 0)
        {
            return nullptr;
        }

        const auto opcode = *reinterpret_cast<const uint8_t*>(target);

        if (opcode != 0xE8)
        {
            Logger::Error(
                "Expected a call instruction for function '{}' "
                "at address {} but instead got 0x{:02X}.",
                p_FunctionName, fmt::ptr(reinterpret_cast<void*>(target)), opcode
            );
            return nullptr;
        }

        const auto displacement = *reinterpret_cast<const int32_t*>(target + 1);

        const uintptr_t targetFunction = target + 5 + displacement;

        return reinterpret_cast<void*>(targetFunction);
    }
};

// Locates a vtable assignment and returns the function pointer
// stored at the requested vtable index.
//
// x64 expects:
//     48 8D 05 xx xx xx xx
//     lea reg, [rip + disp32]
//
// x86 expects:
//     C7 01 xx xx xx xx
//     mov dword ptr [ecx], imm32
template<class T, ECallingConvention Convention = ECallingConvention::Thiscall> class PatternVtableEngineFunction;

template<class ReturnType, class... Args, ECallingConvention Convention>
class PatternVtableEngineFunction<ReturnType(Args...), Convention> final : public EngineFunction<ReturnType(Args...), Convention>
{
  public:
    PatternVtableEngineFunction(const char* p_FunctionName, const char* p_Pattern, const char* p_Mask, size_t p_VtableIndex)
        : EngineFunction<ReturnType(Args...), Convention>(GetTarget(p_FunctionName, p_Pattern, p_Mask, p_VtableIndex))
    {
        if (this->m_Address == nullptr)
        {
            Detail::LogFunctionAddressNotFound(p_FunctionName);
            return;
        }

        Detail::LogFunctionAddressFound(p_FunctionName, this->m_Address);
    }

  private:
    static void* GetTarget(const char* p_FunctionName, const char* p_Pattern, const char* p_Mask, size_t p_VtableIndex)
    {
        const uintptr_t target = Detail::SearchEnginePattern(p_Pattern, p_Mask);

        if (target == 0)
        {
            return nullptr;
        }

        uintptr_t vtableAddress = 0;

#if defined(_M_IX86)
        // C7 01 imm32:
        // mov dword ptr [ecx], offset Vtable
        const auto opcode = *reinterpret_cast<const uint8_t*>(target);

        const auto modRm = *reinterpret_cast<const uint8_t*>(target + 1);

        if (opcode != 0xC7 || modRm != 0x01)
        {
            Logger::Error(
                "Expected an x86 vtable assignment for function "
                "'{}' at address {}, but instead got "
                "0x{:02X} 0x{:02X}.",
                p_FunctionName, fmt::ptr(reinterpret_cast<void*>(target)), opcode, modRm
            );
            return nullptr;
        }

        vtableAddress = *reinterpret_cast<const uint32_t*>(target + 2);
#else
        // 48 8D 05 disp32:
        // lea reg, [rip + disp32]
        const auto rexPrefix = *reinterpret_cast<const uint8_t*>(target);

        if (rexPrefix != 0x48)
        {
            Logger::Error(
                "Expected a REX prefix for vtable function '{}' "
                "at address {} but instead got 0x{:02X}.",
                p_FunctionName, fmt::ptr(reinterpret_cast<void*>(target)), rexPrefix
            );
            return nullptr;
        }

        const auto displacement = *reinterpret_cast<const int32_t*>(target + 3);

        vtableAddress = target + 7 + displacement;
#endif

        const uintptr_t vtableSlot = vtableAddress + p_VtableIndex * sizeof(void*);

        return *reinterpret_cast<void**>(vtableSlot);
    }
};

// Pattern functions

#define PATTERN_FUNCTION_CC(Pattern, Mask, FunctionName, FunctionType, Convention)              \
    ::EngineFunction<FunctionType, ::ECallingConvention::Convention>* Functions::FunctionName = \
        new ::PatternEngineFunction<FunctionType, ::ECallingConvention::Convention>(#FunctionName, Pattern, Mask)

#define PATTERN_CDECL_FUNCTION(Pattern, Mask, FunctionName, FunctionType) PATTERN_FUNCTION_CC(Pattern, Mask, FunctionName, FunctionType, Cdecl)

#define PATTERN_STDCALL_FUNCTION(Pattern, Mask, FunctionName, FunctionType) PATTERN_FUNCTION_CC(Pattern, Mask, FunctionName, FunctionType, Stdcall)

#define PATTERN_FASTCALL_FUNCTION(Pattern, Mask, FunctionName, FunctionType) PATTERN_FUNCTION_CC(Pattern, Mask, FunctionName, FunctionType, Fastcall)

#define PATTERN_THISCALL_FUNCTION(Pattern, Mask, FunctionName, FunctionType) PATTERN_FUNCTION_CC(Pattern, Mask, FunctionName, FunctionType, Thiscall)

// Relative pattern functions

#define PATTERN_RELATIVE_FUNCTION_CC(Pattern, Mask, FunctionName, FunctionType, Convention)     \
    ::EngineFunction<FunctionType, ::ECallingConvention::Convention>* Functions::FunctionName = \
        new ::PatternRelativeEngineFunction<FunctionType, ::ECallingConvention::Convention>(#FunctionName, Pattern, Mask)

#define PATTERN_RELATIVE_CDECL_FUNCTION(Pattern, Mask, FunctionName, FunctionType) \
    PATTERN_RELATIVE_FUNCTION_CC(Pattern, Mask, FunctionName, FunctionType, Cdecl)

#define PATTERN_RELATIVE_STDCALL_FUNCTION(Pattern, Mask, FunctionName, FunctionType) \
    PATTERN_RELATIVE_FUNCTION_CC(Pattern, Mask, FunctionName, FunctionType, Stdcall)

#define PATTERN_RELATIVE_FASTCALL_FUNCTION(Pattern, Mask, FunctionName, FunctionType) \
    PATTERN_RELATIVE_FUNCTION_CC(Pattern, Mask, FunctionName, FunctionType, Fastcall)

#define PATTERN_RELATIVE_THISCALL_FUNCTION(Pattern, Mask, FunctionName, FunctionType) \
    PATTERN_RELATIVE_FUNCTION_CC(Pattern, Mask, FunctionName, FunctionType, Thiscall)

// Vtable pattern functions

#define PATTERN_VTABLE_FUNCTION_CC(Pattern, Mask, VtableIndex, FunctionName, FunctionType, Convention) \
    ::EngineFunction<FunctionType, ::ECallingConvention::Convention>* Functions::FunctionName =        \
        new ::PatternVtableEngineFunction<FunctionType, ::ECallingConvention::Convention>(#FunctionName, Pattern, Mask, VtableIndex)

#define PATTERN_VTABLE_CDECL_FUNCTION(Pattern, Mask, VtableIndex, FunctionName, FunctionType) \
    PATTERN_VTABLE_FUNCTION_CC(Pattern, Mask, VtableIndex, FunctionName, FunctionType, Cdecl)

#define PATTERN_VTABLE_STDCALL_FUNCTION(Pattern, Mask, VtableIndex, FunctionName, FunctionType) \
    PATTERN_VTABLE_FUNCTION_CC(Pattern, Mask, VtableIndex, FunctionName, FunctionType, Stdcall)

#define PATTERN_VTABLE_FASTCALL_FUNCTION(Pattern, Mask, VtableIndex, FunctionName, FunctionType) \
    PATTERN_VTABLE_FUNCTION_CC(Pattern, Mask, VtableIndex, FunctionName, FunctionType, Fastcall)

#define PATTERN_VTABLE_THISCALL_FUNCTION(Pattern, Mask, VtableIndex, FunctionName, FunctionType) \
    PATTERN_VTABLE_FUNCTION_CC(Pattern, Mask, VtableIndex, FunctionName, FunctionType, Thiscall)
