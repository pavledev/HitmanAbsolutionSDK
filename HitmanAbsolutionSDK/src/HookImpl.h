#pragma once

#include <cassert>
#include <unordered_set>
#include <vector>

#include <MinHook.h>

#include "ModSDK.h"
#include "Hook.h"
#include "Logging.h"
#include "ReloadGate.h"
#include "Utils/ProcessUtils.h"

#define MAX_TRAMPOLINES 4096

// TODO: Suspend all threads but the current one when installing or removing hooks.
// This should fix various crashes when hot-reloading.

inline ReloadGate g_HookGate;

class IPluginInterface;

class HookRegistry
{
  private:
    static std::unordered_set<HookBase*>* g_Hooks;

  public:
    static void RegisterHook(HookBase* p_Hook)
    {
        if (g_Hooks == nullptr)
        {
            g_Hooks = new std::unordered_set<HookBase*>();
        }

        g_Hooks->insert(p_Hook);
    }

    static void RemoveHook(HookBase* p_Hook)
    {
        if (g_Hooks == nullptr)
        {
            return;
        }

        g_Hooks->erase(p_Hook);
    }

    static void ClearDetoursWithContext(void* p_Context)
    {
        if (g_Hooks == nullptr)
        {
            return;
        }

        for (auto hook : *g_Hooks)
        {
            hook->RemoveDetoursWithContext(p_Context);
        }
    }

    static void ClearAllDetours()
    {
        if (g_Hooks == nullptr)
        {
            return;
        }

        for (auto hook : *g_Hooks)
        {
            hook->RemoveAllDetours();
        }
    }

    static void DestroyHooks()
    {
        if (g_Hooks == nullptr)
        {
            return;
        }

        for (auto hook : *g_Hooks)
        {
            hook->RemoveAllDetours();
        }

        for (auto hook : *g_Hooks)
        {
            hook->Remove();
        }

        // Drop the per-hook objects and the registry container itself.
        for (auto hook : *g_Hooks)
        {
            delete hook;
        }

        delete g_Hooks;
        g_Hooks = nullptr;
    }
};

#pragma pack(push, 1)
struct DetourTrampoline
{
    DetourTrampoline(uintptr_t p_Address) : m_FunctionAddress(p_Address)
    {
        // NOP
        memset(m_AdditionalInstructions, 0x90, sizeof(m_AdditionalInstructions));

        // 0x48 0xB8 => movabs rax
        m_Mov = 0xB848;

        // 0xFF 0xE0 => jmp rax
        m_Jmp = 0xE0FF;

        m_Pad = 0xCCCCCCCC;
    }

    uint8_t m_AdditionalInstructions[16];
    uint16_t m_Mov;
    uintptr_t m_FunctionAddress;
    uint16_t m_Jmp;
    uint32_t m_Pad;
};
#pragma pack(pop)

class Trampolines
{
  private:
    static DetourTrampoline* g_Trampolines;
    static size_t g_TrampolineCount;

  public:
    static DetourTrampoline*
    CreateTrampoline(uintptr_t p_TargetAddress, void* p_AdditionalInstructions = nullptr, size_t p_AdditionalInstructionsSize = 0)
    {
        if (g_Trampolines == nullptr)
        {
            // Get system information. We use this later to find the first usable free memory region.
            SYSTEM_INFO sysInfo{};
            GetSystemInfo(&sysInfo);

            auto allocAddress = ALIGN_TO(
                reinterpret_cast<uintptr_t>(GetModuleHandleA(nullptr)) + ModSDK::GetInstance().GetImageSize(), sysInfo.dwAllocationGranularity
            );
            uintptr_t allocEnd = allocAddress + sizeof(DetourTrampoline) * MAX_TRAMPOLINES;

            // Iterate through memory regions until we find one that's free and fits our data.
            MEMORY_BASIC_INFORMATION memInfo{};
            auto queryResult = VirtualQuery(reinterpret_cast<void*>(allocAddress), &memInfo, sizeof(memInfo));

            while (queryResult != 0
                   && (memInfo.State != MEM_FREE || reinterpret_cast<uintptr_t>(memInfo.BaseAddress) + memInfo.RegionSize < allocEnd))
            {
                allocAddress = ALIGN_TO(reinterpret_cast<uintptr_t>(memInfo.BaseAddress) + memInfo.RegionSize, sysInfo.dwAllocationGranularity);
                allocEnd = allocAddress + sizeof(DetourTrampoline) * MAX_TRAMPOLINES;

                Logger::Trace(
                    "Memory segment at {} is not suitable. Trying next at {}.", fmt::ptr(memInfo.BaseAddress),
                    fmt::ptr(reinterpret_cast<void*>(allocAddress))
                );

                queryResult = VirtualQuery(reinterpret_cast<void*>(allocAddress), &memInfo, sizeof(memInfo));
            }

            if (queryResult == 0)
            {
                // We didn't find a free memory region. Just allocate wherever and pray for the best.
                Logger::Warn("Could not find a free memory region for trampoline storage. Allocating anywhere and praying.");
                allocAddress = 0;
            }

            Logger::Trace("Attempting to allocate trampoline storage at {}.", fmt::ptr(reinterpret_cast<void*>(allocAddress)));

            g_Trampolines = static_cast<DetourTrampoline*>(VirtualAlloc(
                reinterpret_cast<void*>(allocAddress), sizeof(DetourTrampoline) * MAX_TRAMPOLINES, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE
            ));

            if (g_Trampolines == nullptr)
            {
                Logger::Error(
                    "Trampoline storage allocation failed with error {}. Requested address was {}.", GetLastError(),
                    fmt::ptr(reinterpret_cast<void*>(allocAddress))
                );
                return nullptr;
            }

            g_TrampolineCount = 0;

            Logger::Trace(
                "Allocated trampoline storage at {} (requested address: {}).", fmt::ptr(g_Trampolines),
                fmt::ptr(reinterpret_cast<void*>(allocAddress))
            );
        }

        if (g_TrampolineCount >= MAX_TRAMPOLINES)
        {
            Logger::Error(
                "Could not create trampoline because we have reached the max number of trampolines allowed ({}). You can raise this limit by "
                "modifying MAX_TRAMPOLINES in HookImpl.h.",
                MAX_TRAMPOLINES
            );
            return nullptr;
        }

        auto* trampoline = g_Trampolines + g_TrampolineCount;
        *trampoline = DetourTrampoline(p_TargetAddress);

        if (p_AdditionalInstructions != nullptr && p_AdditionalInstructionsSize > 0)
        {
            memcpy(trampoline->m_AdditionalInstructions, p_AdditionalInstructions, p_AdditionalInstructionsSize);
        }

        ++g_TrampolineCount;

        return trampoline;
    }

    static void ClearTrampolines()
    {
        if (g_Trampolines == nullptr)
        {
            return;
        }

        VirtualFree(g_Trampolines, 0, MEM_RELEASE);
        g_Trampolines = nullptr;
        g_TrampolineCount = 0;
    }
};

template<typename T, ECallingConvention Convention = ECallingConvention::Cdecl> class HookImpl;

template<typename ReturnType, class... Args, ECallingConvention Convention>
class HookImpl<ReturnType(Args...), Convention> : public Hook<ReturnType(Args...), Convention>
{
  protected:
    HookImpl(const char* p_HookName, void* p_Target, typename Hook<ReturnType(Args...), Convention>::EntryFunc_t p_Detour) : m_Target(p_Target)
    {
        InitializeSRWLock(&m_Lock);

        HookRegistry::RegisterHook(this);

        // We push null here because that's what's used by the caller
        // implementation to determine when we've ran out of detours.
        m_Detours.push_back(nullptr);

        if (p_Target == nullptr)
        {
            Logger::Error(
                "Could not find address for hook '{}'. This probably means that the game was updated and the SDK requires changes.", p_HookName
            );
            return;
        }

        // Make sure MinHook is initialized. Re-initialisation on an already-initialised
        // instance is ok; any other failure is fatal for this hook.
        const auto initResult = MH_Initialize();

        if (initResult != MH_OK && initResult != MH_ERROR_ALREADY_INITIALIZED)
        {
            Logger::Error("Could not initialize MinHook for hook '{}'. Error code: {}.", p_HookName, static_cast<int>(initResult));
            return;
        }

        void* original = nullptr;
        auto result = MH_CreateHook(m_Target, reinterpret_cast<void*>(p_Detour), &original);

        if (result != MH_OK)
        {
            Logger::Error("Could not create hook '{}' at address {}. Error code: {}.", p_HookName, fmt::ptr(p_Target), static_cast<int>(result));
            return;
        }

        this->m_OriginalFunc = original;

        /*result = MH_EnableHook(m_Target);

        if (result != MH_OK)
        {
            Logger::Error(
                "Could install detour for hook '{}' at address {}. Error code: {}.", p_HookName, fmt::ptr(p_Target), static_cast<int>(result)
            );
            return;
        }*/

        const auto queueResult = MH_QueueEnableHook(m_Target);

        if (queueResult != MH_OK)
        {
            Logger::Error("Could not queue hook '{}' at address {}. Error code: {}.", p_HookName, fmt::ptr(m_Target), static_cast<int>(queueResult));

            return;
        }

        Logger::Debug("Created hook '{}' at address {}.", p_HookName, fmt::ptr(p_Target));
    }

    HookImpl(const char* p_HookName, typename Hook<ReturnType(Args...), Convention>::OriginalFunc_t p_Original) : m_Target(nullptr)
    {
        InitializeSRWLock(&m_Lock);

        HookRegistry::RegisterHook(this);

        m_Detours.push_back(nullptr);

        if (p_Original == nullptr)
        {
            Logger::Error(
                "Could not find address for hook '{}'. This probably means that the game was updated and the SDK requires changes.", p_HookName
            );
            return;
        }

        this->m_OriginalFunc = reinterpret_cast<void*>(p_Original);

        Logger::Debug("Successfully installed detour for hook '{}' at address {}.", p_HookName, fmt::ptr(p_Original));
    }

  public:
    void Remove() override
    {
        AcquireSRWLockExclusive(&m_Lock);

        for (auto* detour : m_Detours)
        {
            delete detour;
        }

        m_Detours.clear();
        m_Detours.push_back(nullptr);

        if (m_Target != nullptr)
        {
            // MH_DisableHook(m_Target);
            MH_RemoveHook(m_Target);

            this->m_OriginalFunc = m_Target;
        }

        m_Target = nullptr;

        ReleaseSRWLockExclusive(&m_Lock);
    }

    void RemoveDetoursWithContext(void* p_Context) override
    {
        AcquireSRWLockExclusive(&m_Lock);

        for (auto it = m_Detours.begin(); it != m_Detours.end();)
        {
            if (*it == nullptr)
            {
                ++it;
                continue;
            }

            if ((*it)->m_Context == p_Context)
            {
                delete *it;
                it = m_Detours.erase(it);
            }
            else
            {
                ++it;
            }
        }

        ReleaseSRWLockExclusive(&m_Lock);
    }

    void RemoveAllDetours() override
    {
        AcquireSRWLockExclusive(&m_Lock);

        for (auto it = m_Detours.begin(); it != m_Detours.end(); ++it)
        {
            delete *it;
        }

        m_Detours.clear();
        m_Detours.push_back(nullptr);

        ReleaseSRWLockExclusive(&m_Lock);
    }

  protected:
    void AddDetourInternal(void* p_Context, void* p_Detour) override
    {
        // TODO: Do we need any sort of locking here?

        // We remove it first to make sure we only have unique detours
        // in our list. We could use a set to make this easier but iteration
        // performance wouldn't be very great.
        RemoveDetourInternal(p_Detour);

        auto detour = new HookBase::Detour();
        detour->m_DetourFunc = p_Detour;
        detour->m_Context = p_Context;

        AcquireSRWLockExclusive(&m_Lock);

        m_Detours.insert(m_Detours.end() - 1, detour);

        ReleaseSRWLockExclusive(&m_Lock);
    }

    void RemoveDetourInternal(void* p_Detour) override
    {
        AcquireSRWLockExclusive(&m_Lock);

        for (auto it = m_Detours.begin(); it != m_Detours.end();)
        {
            if (*it == nullptr)
            {
                ++it;
                continue;
            }

            if ((*it)->m_DetourFunc == p_Detour)
            {
                delete *it;
                it = m_Detours.erase(it);
            }
            else
            {
                ++it;
            }
        }

        ReleaseSRWLockExclusive(&m_Lock);
    }

    HookBase::Detour** GetDetours() override
    {
        return m_Detours.data();
    }

    void LockForCall() override
    {
        AcquireSRWLockShared(&m_Lock);
    }

    void UnlockForCall() override
    {
        ReleaseSRWLockShared(&m_Lock);
    }

  protected:
    std::vector<HookBase::Detour*> m_Detours;
    void* m_Target;
    SRWLOCK m_Lock;
};

template<typename T, ECallingConvention Convention = ECallingConvention::Cdecl> class PatternHook;

template<typename ReturnType, class... Args, ECallingConvention Convention>
class PatternHook<ReturnType(Args...), Convention> final : public HookImpl<ReturnType(Args...), Convention>
{
  public:
    PatternHook(
        const char* p_HookName, const char* p_Pattern, const char* p_Mask, typename Hook<ReturnType(Args...), Convention>::EntryFunc_t p_Detour
    )
        : HookImpl<ReturnType(Args...), Convention>(p_HookName, GetTarget(p_Pattern, p_Mask), p_Detour)
    {}

  private:
    void* GetTarget(const char* p_Pattern, const char* p_Mask) const
    {
        const auto* pattern = reinterpret_cast<const uint8_t*>(p_Pattern);
        return reinterpret_cast<void*>(
            util::ProcessUtils::SearchPattern(ModSDK::GetInstance().GetModuleBase(), ModSDK::GetInstance().GetSizeOfCode(), pattern, p_Mask)
        );
    }
};

template<typename T, ECallingConvention Convention = ECallingConvention::Cdecl> class PatternCallHook;

template<typename ReturnType, class... Args, ECallingConvention Convention>
class PatternCallHook<ReturnType(Args...), Convention> final : public HookImpl<ReturnType(Args...), Convention>
{
  public:
    PatternCallHook(
        const char* p_HookName, const char* p_Pattern, const char* p_Mask, typename Hook<ReturnType(Args...), Convention>::EntryFunc_t p_Detour
    )
        : HookImpl<ReturnType(Args...), Convention>(p_HookName, InstallDetourAndGetOriginal(p_HookName, p_Pattern, p_Mask, p_Detour))
    {}

    void Remove() override
    {
        AcquireSRWLockExclusive(&this->m_Lock);

        for (auto* detour : this->m_Detours)
        {
            delete detour;
        }

        this->m_Detours.clear();
        this->m_Detours.push_back(nullptr);

        if (m_Target != 0 && this->m_OriginalFunc != nullptr)
        {
            // Restore the original call.
            const ptrdiff_t distance = reinterpret_cast<uintptr_t>(this->m_OriginalFunc) - (m_Target + 5);

            // We don't need to check if this is within INT32 bounds here because it should always be.
            // Cast down to int and rewrite the call offset.
            DWORD oldProtect;
            VirtualProtect(reinterpret_cast<void*>(m_Target), 5, PAGE_EXECUTE_READWRITE, &oldProtect);

            *reinterpret_cast<int32_t*>(m_Target + 1) = static_cast<int32_t>(distance);

            VirtualProtect(reinterpret_cast<void*>(m_Target), 5, oldProtect, nullptr);
        }

        m_Target = 0;

        ReleaseSRWLockExclusive(&this->m_Lock);
    }

  private:
    typename Hook<ReturnType(Args...), Convention>::OriginalFunc_t InstallDetourAndGetOriginal(
        const char* p_HookName, const char* p_Pattern, const char* p_Mask, typename Hook<ReturnType(Args...), Convention>::EntryFunc_t p_Detour
    )
    {
        const auto* pattern = reinterpret_cast<const uint8_t*>(p_Pattern);
        m_Target = util::ProcessUtils::SearchPattern(ModSDK::GetInstance().GetModuleBase(), ModSDK::GetInstance().GetSizeOfCode(), pattern, p_Mask);

        // We expect this to be a CALL (0xE8) instruction.
        if (m_Target != 0 && *reinterpret_cast<uint8_t*>(m_Target) != 0xE8)
        {
            Logger::Error(
                "Expected a call instruction for hook '{}' at address {} but instead got 0x{:02X}.", p_HookName,
                fmt::ptr(reinterpret_cast<void*>(m_Target)), *reinterpret_cast<uint8_t*>(m_Target)
            );
            return nullptr;
        }

        if (m_Target == 0)
        {
            return nullptr;
        }

        // Real function location will be at instruction + 5 + offset.
        // Offset is a 32-bit signed integer at instruction + 1.
        const uintptr_t originalFunction = m_Target + 5 + *reinterpret_cast<int32_t*>(m_Target + 1);

        // Get the distance between this call and our detour.
        ptrdiff_t distance = reinterpret_cast<uintptr_t>(p_Detour) - (m_Target + 5);

        // If the distance is out of the int32 range we must create a trampoline.
        if (distance >= INT32_MAX || distance <= INT32_MIN)
        {
            Logger::Trace(
                "Detour for hook '{}' is too far from the original call ({} - {} = {}). Creating trampoline.", p_HookName, fmt::ptr(p_Detour),
                fmt::ptr(reinterpret_cast<void*>(m_Target + 5)), distance
            );

            const auto trampolineAddress = Trampolines::CreateTrampoline(reinterpret_cast<uintptr_t>(p_Detour));

            // Failed to create trampoline.
            if (trampolineAddress == 0)
            {
                return nullptr;
            }

            distance = reinterpret_cast<uintptr_t>(trampolineAddress) - (m_Target + 5);

            // Sanity check again.
            if (distance >= INT32_MAX || distance <= INT32_MIN)
            {
                Logger::Error(
                    "Trampoline for hook '{}' is too far from the original call ({} - {} = {}).", p_HookName,
                    fmt::ptr(reinterpret_cast<void*>(trampolineAddress)), fmt::ptr(reinterpret_cast<void*>(m_Target + 5)), distance
                );
                return nullptr;
            }
        }

        // Cast down to int and rewrite the call offset.
        DWORD oldProtect;
        VirtualProtect(reinterpret_cast<void*>(m_Target), 5, PAGE_EXECUTE_READWRITE, &oldProtect);

        *reinterpret_cast<int32_t*>(m_Target + 1) = static_cast<int32_t>(distance);

        VirtualProtect(reinterpret_cast<void*>(m_Target), 5, oldProtect, nullptr);

        return reinterpret_cast<typename Hook<ReturnType(Args...), Convention>::OriginalFunc_t>(originalFunction);
    }

    uintptr_t m_Target;
};

template<typename T, ECallingConvention Convention = ECallingConvention::Cdecl> class PatternRelativeCallHook;

template<typename ReturnType, class... Args, ECallingConvention Convention>
class PatternRelativeCallHook<ReturnType(Args...), Convention> final : public HookImpl<ReturnType(Args...), Convention>
{
  public:
    PatternRelativeCallHook(
        const char* p_HookName, const char* p_Pattern, const char* p_Mask, typename Hook<ReturnType(Args...), Convention>::EntryFunc_t p_Detour
    )
        : HookImpl<ReturnType(Args...), Convention>(p_HookName, GetTarget(p_HookName, p_Pattern, p_Mask), p_Detour)
    {}

  private:
    void* GetTarget(const char* p_HookName, const char* p_Pattern, const char* p_Mask) const
    {
        const auto* pattern = reinterpret_cast<const uint8_t*>(p_Pattern);
        auto target =
            util::ProcessUtils::SearchPattern(ModSDK::GetInstance().GetModuleBase(), ModSDK::GetInstance().GetSizeOfCode(), pattern, p_Mask);

        // We expect this to be a CALL (0xE8) instruction.
        if (target != 0 && *reinterpret_cast<uint8_t*>(target) != 0xE8)
        {
            Logger::Error(
                "Expected a call instruction for hook '{}' at address {} but instead got 0x{:02X}.", p_HookName,
                fmt::ptr(reinterpret_cast<void*>(target)), *reinterpret_cast<uint8_t*>(target)
            );
            return nullptr;
        }

        if (target == 0)
        {
            return nullptr;
        }

        const uintptr_t originalFunction = target + 5 + *reinterpret_cast<int32_t*>(target + 1);

        return reinterpret_cast<void*>(originalFunction);
    }
};

template<typename T> class ThiscallPatternVtableHook;

template<typename ReturnType, class... Args>
class ThiscallPatternVtableHook<ReturnType(Args...)> final : public HookImpl<ReturnType(Args...), ECallingConvention::Thiscall>
{
  private:
    using Base = HookImpl<ReturnType(Args...), ECallingConvention::Thiscall>;
    using HookType = Hook<ReturnType(Args...), ECallingConvention::Thiscall>;

  public:
    ThiscallPatternVtableHook(
        const char* p_HookName, const char* p_Pattern, const char* p_Mask, size_t p_VtableIndex, typename HookType::EntryFunc_t p_Detour
    )
        : Base(p_HookName, GetTarget(p_HookName, p_Pattern, p_Mask, p_VtableIndex), p_Detour)
    {}

  private:
    void* GetTarget(const char* p_HookName, const char* p_Pattern, const char* p_Mask, size_t p_VtableIndex) const
    {
        const auto* pattern = reinterpret_cast<const uint8_t*>(p_Pattern);

        const auto target =
            util::ProcessUtils::SearchPattern(ModSDK::GetInstance().GetModuleBase(), ModSDK::GetInstance().GetSizeOfCode(), pattern, p_Mask);

        if (target == 0)
        {
            return nullptr;
        }

        if (*reinterpret_cast<uint8_t*>(target) != 0xC7)
        {
            Logger::Error(
                "Expected an x86 vtable assignment for hook '{}' at address {} "
                "but instead got 0x{:02X}.",
                p_HookName, fmt::ptr(reinterpret_cast<void*>(target)), *reinterpret_cast<uint8_t*>(target)
            );

            return nullptr;
        }

        const uintptr_t vtableAddress = *reinterpret_cast<uint32_t*>(target + 2);

        const uintptr_t functionAddress = vtableAddress + p_VtableIndex * sizeof(void*);

        return *reinterpret_cast<void**>(functionAddress);
    }
};

template<typename T, ECallingConvention Convention = ECallingConvention::Cdecl> class ModuleHook;

template<typename ReturnType, class... Args, ECallingConvention Convention>
class ModuleHook<ReturnType(Args...), Convention> final : public HookImpl<ReturnType(Args...), Convention>
{
  public:
    ModuleHook(
        const char* p_HookName, const char* p_ModuleName, const char* p_FunctionName,
        typename Hook<ReturnType(Args...), Convention>::EntryFunc_t p_Detour
    )
        : HookImpl<ReturnType(Args...), Convention>(p_HookName, GetTarget(p_HookName, p_ModuleName, p_FunctionName), p_Detour)
    {}

  private:
    void* GetTarget(const char* p_HookName, const char* p_ModuleName, const char* p_FunctionName) const
    {
        const auto module = LoadLibraryA(p_ModuleName);

        if (module == nullptr)
        {
            Logger::Error("Could not load requested module '{}' for hook '{}' (error: {}).", p_ModuleName, p_HookName, GetLastError());
            return nullptr;
        }

        const auto function = GetProcAddress(module, p_FunctionName);

        if (function == nullptr)
        {
            Logger::Error(
                "Could not find requested function '{}' in module '{}' for hook '{}' (error: {}).", p_FunctionName, p_ModuleName, p_HookName,
                GetLastError()
            );
            return nullptr;
        }

        return reinterpret_cast<void*>(function);
    }
};

#define PATTERN_HOOK(Pattern, Mask, HookName, HookType)                                                        \
    PatternHook<HookType> PatternHook<HookType>* Hooks::HookName = new PatternHook<HookType>(                  \
        #HookName, Pattern, Mask, (typename Hook<HookType>::OriginalFunc_t)[]<class... Args>(Args... p_Args) { \
            static auto* hook = Hooks::HookName;                                                               \
            if (auto ticket = g_HookGate.TryEnter(); ticket)                                                   \
            {                                                                                                  \
                return hook->Call(p_Args...);                                                                  \
            }                                                                                                  \
            return hook->CallOriginal(p_Args...);                                                              \
        }                                                                                                      \
    )

#define PATTERN_THISCALL_VTABLE_HOOK(Pattern, Mask, VtableIndex, HookName, HookType)                                          \
    struct HookName##_HookTag                                                                                                 \
    {                                                                                                                         \
        static auto* Get()                                                                                                    \
        {                                                                                                                     \
            return Hooks::HookName;                                                                                           \
        }                                                                                                                     \
    };                                                                                                                        \
                                                                                                                              \
    ThiscallHook<HookType>* Hooks::HookName = new ThiscallPatternVtableHook<HookType>(                                        \
        #HookName, Pattern, Mask, VtableIndex, &HookEntry<HookType, ECallingConvention::Thiscall, HookName##_HookTag>::Invoke \
    )

#define PATTERN_CALL_HOOK(Pattern, Mask, HookName, HookType)                                                   \
    PatternCallHook<HookType>* Hooks::HookName = new PatternCallHook<HookType>(                                \
        #HookName, Pattern, Mask, (typename Hook<HookType>::OriginalFunc_t)[]<class... Args>(Args... p_Args) { \
            static auto* hook = Hooks::HookName;                                                               \
            if (auto ticket = g_HookGate.TryEnter(); ticket)                                                   \
            {                                                                                                  \
                return hook->Call(p_Args...);                                                                  \
            }                                                                                                  \
            return hook->CallOriginal(p_Args...);                                                              \
        }                                                                                                      \
    )

#define PATTERN_RELATIVE_CALL_HOOK(Pattern, Mask, HookName, HookType)                                          \
    PatternRelativeCallHook<HookType> Hooks::HookName = new PatternRelativeCallHook<HookType>(                 \
        #HookName, Pattern, Mask, (typename Hook<HookType>::OriginalFunc_t)[]<class... Args>(Args... p_Args) { \
            static auto* hook = Hooks::HookName;                                                               \
            if (auto ticket = g_HookGate.TryEnter(); ticket)                                                   \
            {                                                                                                  \
                return hook->Call(p_Args...);                                                                  \
            }                                                                                                  \
            return hook->CallOriginal(p_Args...);                                                              \
        }                                                                                                      \
    )

#define MODULE_HOOK(ModuleName, FunctionName, HookName, HookType)                                                         \
    ModuleHook<HookType>* Hooks::HookName = new ModuleHook<HookType>(                                                     \
        #HookName, ModuleName, FunctionName, (typename Hook<HookType>::OriginalFunc_t)[]<class... Args>(Args... p_Args) { \
            static auto* hook = Hooks::HookName;                                                                          \
            if (auto ticket = g_HookGate.TryEnter(); ticket)                                                              \
            {                                                                                                             \
                return hook->Call(p_Args...);                                                                             \
            }                                                                                                             \
            return hook->CallOriginal(p_Args...);                                                                         \
        }                                                                                                                 \
    )

template<typename T> using CdeclPatternHook = PatternHook<T, ECallingConvention::Cdecl>;
template<typename T> using StdcallPatternHook = PatternHook<T, ECallingConvention::Stdcall>;
template<typename T> using FastcallPatternHook = PatternHook<T, ECallingConvention::Fastcall>;
template<typename T> using ThiscallPatternHook = PatternHook<T, ECallingConvention::Thiscall>;

template<typename T, ECallingConvention Convention, typename HookTag> struct HookEntry;

template<typename ReturnType, typename... Args, typename HookTag> struct HookEntry<ReturnType(Args...), ECallingConvention::Cdecl, HookTag>
{
    static ReturnType __cdecl Invoke(Args... p_Args)
    {
        auto* hook = HookTag::Get();

        if (!g_HookGate.TryEnter())
        {
            return hook->CallOriginal(p_Args...);
        }

        if constexpr (std::is_void_v<ReturnType>)
        {
            hook->Call(p_Args...);
            g_HookGate.Leave();
        }
        else
        {
            ReturnType result = hook->Call(p_Args...);

            g_HookGate.Leave();

            return result;
        }
    }
};

template<typename ReturnType, typename... Args, typename HookTag> struct HookEntry<ReturnType(Args...), ECallingConvention::Stdcall, HookTag>
{
    static ReturnType __stdcall Invoke(Args... p_Args)
    {
        auto* hook = HookTag::Get();

        if (!g_HookGate.TryEnter())
        {
            return hook->CallOriginal(p_Args...);
        }

        if constexpr (std::is_void_v<ReturnType>)
        {
            hook->Call(p_Args...);
            g_HookGate.Leave();
        }
        else
        {
            ReturnType result = hook->Call(p_Args...);

            g_HookGate.Leave();

            return result;
        }
    }
};

template<typename ReturnType, typename... Args, typename HookTag> struct HookEntry<ReturnType(Args...), ECallingConvention::Fastcall, HookTag>
{
    static ReturnType __fastcall Invoke(Args... p_Args)
    {
        auto* hook = HookTag::Get();

        if (!g_HookGate.TryEnter())
        {
            return hook->CallOriginal(p_Args...);
        }

        if constexpr (std::is_void_v<ReturnType>)
        {
            hook->Call(p_Args...);
            g_HookGate.Leave();
        }
        else
        {
            ReturnType result = hook->Call(p_Args...);

            g_HookGate.Leave();

            return result;
        }
    }
};

template<typename ReturnType, typename ThisType, typename... Args, typename HookTag>
struct HookEntry<ReturnType(ThisType, Args...), ECallingConvention::Thiscall, HookTag>
{
    static ReturnType __fastcall Invoke(ThisType p_This, void*, Args... p_Args)
    {
        auto* hook = HookTag::Get();

        if (!g_HookGate.TryEnter())
        {
            return hook->CallOriginal(p_This, p_Args...);
        }

        if constexpr (std::is_void_v<ReturnType>)
        {
            hook->Call(p_This, p_Args...);
            g_HookGate.Leave();
        }
        else
        {
            ReturnType result = hook->Call(p_This, p_Args...);

            g_HookGate.Leave();

            return result;
        }
    }
};

#define PATTERN_CDECL_HOOK(Pattern, Mask, HookName, HookType) \
    struct HookName##_HookTag                                 \
    {                                                         \
        static auto* Get()                                    \
        {                                                     \
            return Hooks::HookName;                           \
        }                                                     \
    };                                                        \
                                                              \
    CdeclHook<HookType>* Hooks::HookName =                    \
        new CdeclPatternHook<HookType>(#HookName, Pattern, Mask, &HookEntry<HookType, ECallingConvention::Cdecl, HookName##_HookTag>::Invoke)

#define PATTERN_STDCALL_HOOK(Pattern, Mask, HookName, HookType) \
    struct HookName##_HookTag                                   \
    {                                                           \
        static auto* Get()                                      \
        {                                                       \
            return Hooks::HookName;                             \
        }                                                       \
    };                                                          \
                                                                \
    StdcallHook<HookType>* Hooks::HookName =                    \
        new StdcallPatternHook<HookType>(#HookName, Pattern, Mask, &HookEntry<HookType, ECallingConvention::Stdcall, HookName##_HookTag>::Invoke)

#define PATTERN_FASTCALL_HOOK(Pattern, Mask, HookName, HookType) \
    struct HookName##_HookTag                                    \
    {                                                            \
        static auto* Get()                                       \
        {                                                        \
            return Hooks::HookName;                              \
        }                                                        \
    };                                                           \
                                                                 \
    FastcallHook<HookType>* Hooks::HookName =                    \
        new FastcallPatternHook<HookType>(#HookName, Pattern, Mask, &HookEntry<HookType, ECallingConvention::Fastcall, HookName##_HookTag>::Invoke)

#define PATTERN_THISCALL_HOOK(Pattern, Mask, HookName, HookType) \
    struct HookName##_HookTag                                    \
    {                                                            \
        static auto* Get()                                       \
        {                                                        \
            return Hooks::HookName;                              \
        }                                                        \
    };                                                           \
                                                                 \
    ThiscallHook<HookType>* Hooks::HookName =                    \
        new ThiscallPatternHook<HookType>(#HookName, Pattern, Mask, &HookEntry<HookType, ECallingConvention::Thiscall, HookName##_HookTag>::Invoke)
