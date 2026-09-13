#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>

#include <fmt/format.h>

#include "SDK.h"
#include "Utils/ProcessUtils.h"
#include "Logging.h"

template<class T> T PatternGlobal(const char* p_GlobalName, const char* p_Pattern, const char* p_Mask, ptrdiff_t p_Offset)
{
    static_assert(std::is_pointer_v<T>, "Global type is not a pointer type.");

    const auto* pattern = reinterpret_cast<const uint8_t*>(p_Pattern);

    const uintptr_t target =
        util::ProcessUtils::SearchPattern(SDK::GetInstance().GetModuleBase(), SDK::GetInstance().GetSizeOfCode(), pattern, p_Mask);

    if (target == 0)
    {
        Logger::Error(
            "Could not find address for global '{}'. "
            "This probably means that the game was updated "
            "and the SDK requires changes.",
            p_GlobalName
        );

        return nullptr;
    }

    const uintptr_t addressOperand = target + p_Offset;

    uintptr_t finalAddress;

#if defined(_M_IX86)
    // x86 instructions generally contain the absolute address:
    //
    // A1 xx xx xx xx
    // mov eax, [absolute address]
    //
    // 8B 0D xx xx xx xx
    // mov ecx, [absolute address]
    finalAddress = *reinterpret_cast<const uint32_t*>(addressOperand);
#else
    // x64 instructions generally contain a signed displacement
    // relative to the end of the disp32 operand:
    //
    // 48 8B 0D xx xx xx xx
    // mov rcx, [rip + disp32]
    const int32_t displacement = *reinterpret_cast<const int32_t*>(addressOperand);

    finalAddress = addressOperand + sizeof(displacement) + displacement;
#endif

    Logger::Debug("Successfully located global '{}' at address {}.", p_GlobalName, fmt::ptr(reinterpret_cast<void*>(finalAddress)));

    return reinterpret_cast<T>(finalAddress);
}

#define PATTERN_GLOBAL(Pattern, Mask, Offset, GlobalType, GlobalName) \
    GlobalType Globals::GlobalName = PatternGlobal<GlobalType>(#GlobalName, Pattern, Mask, Offset)
