#pragma once

#include <span>

#include <imgui.h>

#include "IModSDK.h"
#include "ModManager.h"
#include "Hooks.h"

class DirectXRenderer;
class ImGuiRenderer;
class DebugConsole;
class ResourcePatcher;

namespace UI
{
    class ModSelector;
    class MainMenu;
    class Console;
    class Settings;
}

class ModSDK : public IModSDK
{
  public:
    static ModSDK& GetInstance();

    void Startup();
    void Cleanup();

    void OnEngineInitialized();
    void OnEngineUninitialized();

    void OnModLoaded(const std::string& p_Name, IModInterface* p_Mod, bool p_LiveLoad);
    void OnModUnloading(const std::string& p_Name, IModInterface* p_Mod);
    void OnModUnloaded(const std::string& p_Name);

    void OnDrawMenu() const;
    void OnDrawUI(bool p_HasFocus) const;
    void OnDraw3D() const;
    void OnDepthDraw3D() const;
    void OnDraw2D() const;

    void Log(spdlog::level::level_enum p_Level, std::string_view p_Msg) override;

    bool PatchCode(const char* p_Pattern, const char* p_Mask, void* p_NewCode, size_t p_CodeSize, ptrdiff_t p_Offsp_TargetOffsetet) override;
    bool PatchCodeStoreOriginal(
        const char* p_Pattern, const char* p_Mask, void* p_NewCode, size_t p_CodeSize, ptrdiff_t p_TargetOffset, void* p_OriginalCode
    ) override;

    void
    GetModSetting(IModInterface* p_Mod, const ZString& p_Section, const ZString& p_Name, const ZString& p_DefaultValue, ZString& p_OutValue) override;

    int64_t GetModSettingInt(IModInterface* p_Mod, const ZString& p_Section, const ZString& p_Name, int64_t p_DefaultValue) override;

    uint64_t GetModSettingUInt(IModInterface* p_Mod, const ZString& p_Section, const ZString& p_Name, uint64_t p_DefaultValue) override;

    double GetModSettingDouble(IModInterface* p_Mod, const ZString& p_Section, const ZString& p_Name, double p_DefaultValue) override;

    bool GetModSettingBool(IModInterface* p_Mod, const ZString& p_Section, const ZString& p_Name, bool p_DefaultValue) override;

    void SetModSetting(IModInterface* p_Mod, const ZString& p_Section, const ZString& p_Name, const ZString& p_Value) override;

    void SetModSettingInt(IModInterface* p_Mod, const ZString& p_Section, const ZString& p_Name, int64_t p_Value) override;

    void SetModSettingUInt(IModInterface* p_Mod, const ZString& p_Section, const ZString& p_Name, uint64_t p_Value) override;

    void SetModSettingDouble(IModInterface* p_Mod, const ZString& p_Section, const ZString& p_Name, double p_Value) override;

    void SetModSettingBool(IModInterface* p_Mod, const ZString& p_Section, const ZString& p_Name, bool p_Value) override;

    bool HasModSetting(IModInterface* p_Mod, const ZString& p_Section, const ZString& p_Name) override;
    void RemoveModSetting(IModInterface* p_Mod, const ZString& p_Section, const ZString& p_Name) override;
    void ReloadModSettings(IModInterface* p_Mod) override;

    bool WorldToScreen(const SVector3& p_WorldPos, SVector2& p_Out) override;
    bool ScreenToWorld(const SVector2& p_ScreenPos, SVector3& p_OutWorldPos, SVector3& p_OutDirection) override;

    void RequestUIFocus() override;
    void ReleaseUIFocus() override;

    const char* GetResourceID(uint64_t p_RuntimeResourceID) const override;
    uint64_t GetRuntimeResourceID(const std::string& p_ResourceID) const override;

    const std::map<int32_t, std::string>& GetEnum(const std::string& p_TypeName) const override;

    const std::string& GetPropertyName(uint32_t p_PropertyID) const override;

    std::span<const std::byte> GetResource(int32_t p_ResourceID) const override;
    std::string_view GetTextResource(int32_t p_ResourceID) const override;

    bool InstallDynamicResourceLibrary(
        const ZRuntimeResourceID& p_HeaderLibraryRuntimeResourceID, ZDynamicResourceLibrary*& p_DynamicResourceLibrary,
        ZRuntimeResourceID& p_TempRuntimeResourceID, uint32_t p_EntityCount = 1
    ) const override;

    bool InstallDynamicResourceLibrary(
        const std::string& p_HeaderLibraryResourceID, ZDynamicResourceLibrary*& p_DynamicResourceLibrary, ZRuntimeResourceID& p_TempRuntimeResourceID,
        const uint32_t p_EntityCount = 1
    ) const override;

    bool CreateAndInstallDynamicResourceLibrary(
        const std::string& p_ResourceID, ZDynamicResourceLibrary*& p_DynamicResourceLibrary, ZRuntimeResourceID& p_TempRuntimeResourceID,
        const uint32_t p_EntityCount = 1
    ) override;

    std::shared_ptr<DirectXRenderer> GetDirectXRenderer() const
    {
        return m_DirectXRenderer;
    }

    std::shared_ptr<ImGuiRenderer> GetImGuiRenderer() const
    {
        return m_ImGuiRenderer;
    }

    std::shared_ptr<ModManager> GetModManager() const
    {
        return m_ModManager;
    }

    std::shared_ptr<UI::MainMenu> GetUIMainMenu() const
    {
        return m_UIMainMenu;
    }

    std::shared_ptr<UI::ModSelector> GetUIModSelector() const
    {
        return m_UIModSelector;
    }

    std::shared_ptr<UI::Console> GetUIConsole() const
    {
        return m_UIConsole;
    }

    uint8_t GetConsoleScanCode() const
    {
        return m_ConsoleScanCode;
    }

    uint8_t GetUIToggleScanCode() const
    {
        return m_UIToggleScanCode;
    }

    bool HasShownUIToggleWarning() const
    {
        return m_HasShownUIToggleWarning.load(std::memory_order_acquire);
    }

    void SetHasShownUIToggleWarning();

    std::shared_ptr<ResourcePatcher> GetResourcePatcher() const
    {
        return m_ResourcePatcher;
    }

    uintptr_t GetModuleBase() const
    {
        return m_ModuleBase;
    }

    uint32_t GetSizeOfCode() const
    {
        return m_SizeOfCode;
    }

    uint32_t GetImageSize() const
    {
        return m_ImageSize;
    }

  private:
    ModSDK();
    ModSDK(const ModSDK& other) = delete;
    ModSDK& operator=(const ModSDK& other) = delete;

    bool PatchCodeInternal(const char* p_Pattern, const char* p_Mask, void* p_NewCode, size_t p_CodeSize, ptrdiff_t p_Offset, void* p_OriginalCode);

    void LoadConfiguration();
    void SetConfigurationValue(const std::string& p_Section, const std::string& p_Key, const std::string& p_Value);

    static std::pair<uint32_t, std::string> RequestLatestVersion();
    static void ShowVersionNotice(const std::string& p_Version);
    void SkipVersionUpdate(const std::string& p_Version);
    bool CheckForUpdates() const;

    static void OnConsoleCommand(void* p_Context, TArray<ZString>& p_Args);

    DECLARE_THISCALL_DETOUR_WITH_CONTEXT(ModSDK, bool, ZHitman5Module_Initialize, ZHitman5Module* p_Hitman5Module);
    DECLARE_THISCALL_DETOUR_WITH_CONTEXT(
        ModSDK, bool, ZEngineAppCommon_Initialize, ZEngineAppCommon* p_EngineAppCommon, const SRenderDestinationDesc& p_Description
    );
    DECLARE_THISCALL_DETOUR_WITH_CONTEXT(ModSDK, void, ZEngineAppCommon_Uninitialize, ZEngineAppCommon* p_EngineAppCommon);

    DECLARE_THISCALL_DETOUR_WITH_CONTEXT(ModSDK, void, ZRenderDevice_Present, ZRenderDevice* p_RenderDevice);
    DECLARE_THISCALL_DETOUR_WITH_CONTEXT(
        ModSDK, void, ZRenderSwapChain_Resize, ZRenderSwapChain* p_RenderSwapChain, const SRenderDestinationDesc* p_Description
    );

    bool m_ReadHMAIni = true;
    bool m_EnableResourcePatching = false;

    bool m_UIEnabled = true;
    uint8_t m_ConsoleScanCode = 0x29;  // Grave / Tilde key
    uint8_t m_UIToggleScanCode = 0x57; // F11
    std::atomic<bool> m_HasShownUIToggleWarning = false;

    std::shared_ptr<DirectXRenderer> m_DirectXRenderer;
    std::shared_ptr<ImGuiRenderer> m_ImGuiRenderer;

    std::shared_ptr<ModManager> m_ModManager;

    std::shared_ptr<UI::MainMenu> m_UIMainMenu;
    std::shared_ptr<UI::ModSelector> m_UIModSelector;
    std::shared_ptr<UI::Console> m_UIConsole;

    std::shared_ptr<ResourcePatcher> m_ResourcePatcher;

    uintptr_t m_ModuleBase;
    uint32_t m_SizeOfCode;
    uint32_t m_ImageSize;

#if _DEBUG
    std::shared_ptr<DebugConsole> m_DebugConsole{};
#endif

    std::string m_IgnoredVersion;
    bool m_DisableUpdateCheck = false;
};
