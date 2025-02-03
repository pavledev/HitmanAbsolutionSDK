#pragma once

#include <MinHook.h>

#define MINI_CASE_SENSITIVE

#include "ini.h"

#include "Logger.h"
#include "SDK.h"
#include "Utility/StringUtility.h"

class ModInterface
{
public:
    virtual ~ModInterface() = default;

    virtual void Initialize()
    {
        if (MH_Initialize() != MH_OK)
        {
            Logger::GetInstance().Log(Logger::Level::Error, "Failed to initialize MinHook!");
        }
    }

    virtual void OnEngineInitialized()
    {
    }

    virtual void OnDrawUI(const bool hasFocus)
    {
    }

    virtual void OnDraw3D()
    {
    }

    virtual void OnDrawMenu()
    {
    }

    virtual void SetupUI()
    {
        SDK& sdk = SDK::GetInstance();
        ImGuiContext* imGuiContext = sdk.GetImGuiContext();

        if (!imGuiContext)
        {
            return;
        }

        ImGui::SetCurrentContext(imGuiContext);
        ImGui::SetAllocatorFunctions(sdk.GetImGuiMemAllocFunc(), sdk.GetImGuiMemFreeFunc(), sdk.GetImGuiUserDataAllocator());
    }

    void LoadConfiguration(const std::string& modName);

protected:
    __declspec(dllexport) void AddBindings();
    std::string GenerateBindingExpression(const std::string& actionName, const std::string& keys);

    std::string modName;
    mINI::INIStructure iniStructure;
};

using GetModInterface_t = ModInterface * (*)();

#define DECLARE_MOD(ModClass) \
    extern "C" __declspec(dllexport) ModInterface* GetModInterface(); \
    \
    ModClass* GetModInstance() \
    { \
        return static_cast<ModClass*>(GetModInterface()); \
    }

#define DEFINE_MOD(ModClass) \
    static ModInterface* ModClass ## Singleton = nullptr; \
    \
    extern "C" __declspec(dllexport) ModInterface* GetModInterface() \
    { \
        if (!ModClass ## Singleton) \
            ModClass ## Singleton = new ModClass(); \
        \
        return ModClass ## Singleton; \
    }
