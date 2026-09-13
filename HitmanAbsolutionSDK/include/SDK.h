#pragma once

#include <span>

#include <imgui.h>

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

class SDK
{
  public:
    HitmanAbsolutionSDK_API static SDK& GetInstance();
    void Setup();
    void Cleanup();

    void OnEngineInitialized();
    void OnEngineUninitialized();
    void OnModLoaded(const std::string& p_Name, IModInterface* m_ModInterface, const bool m_LiveLoad);
    void OnDrawUI(const bool hasFocus);
    void OnDraw3D();
    void OnDrawMenu();

    HitmanAbsolutionSDK_API const char* GetResourceID(uint64_t p_RuntimeResourceID) const;
    HitmanAbsolutionSDK_API uint64_t GetRuntimeResourceID(const std::string& p_ResourceID) const;

    HitmanAbsolutionSDK_API const std::map<int, std::string>& GetEnum(const std::string& p_TypeName);

    HitmanAbsolutionSDK_API const std::string& GetPropertyName(uint32_t p_PropertyID) const;

    HitmanAbsolutionSDK_API std::span<const std::byte> GetResource(int32_t p_ResourceID) const;
    HitmanAbsolutionSDK_API std::string_view GetTextResource(int32_t p_ResourceID) const;

    HitmanAbsolutionSDK_API bool CreateAndInstallDynamicResourceLibrary(
        const std::string& p_ResourceID, ZDynamicResourceLibrary*& p_DynamicResourceLibrary, ZRuntimeResourceID& p_TempRuntimeResourceID,
        const uint32_t p_EntityCount = 1
    );

    HitmanAbsolutionSDK_API ImGuiContext* GetImGuiContext();
    HitmanAbsolutionSDK_API ImGuiMemAllocFunc GetImGuiMemAllocFunc();
    HitmanAbsolutionSDK_API ImGuiMemFreeFunc GetImGuiMemFreeFunc();
    HitmanAbsolutionSDK_API void* GetImGuiUserDataAllocator();
    HitmanAbsolutionSDK_API ImFont* GetRegularFont();
    HitmanAbsolutionSDK_API ImFont* GetBoldFont();

    HitmanAbsolutionSDK_API std::shared_ptr<DirectXRenderer> GetDirectXRenderer() const;
    HitmanAbsolutionSDK_API std::shared_ptr<ImGuiRenderer> GetImGuiRenderer() const;

    std::shared_ptr<ModManager> GetModManager() const;

    std::shared_ptr<UI::ModSelector> GetModSelector() const;
    std::shared_ptr<UI::Console> GetConsole() const;
    std::shared_ptr<UI::Settings> GetSettings() const;

    std::shared_ptr<ResourcePatcher> GetResourcePatcher() const;

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
    SDK();
    SDK(const SDK& other) = delete;
    SDK& operator=(const SDK& other) = delete;

    DECLARE_THISCALL_DETOUR_WITH_CONTEXT(SDK, bool, ZHitman5Module_Initialize, ZHitman5Module* p_Hitman5Module);
    DECLARE_THISCALL_DETOUR_WITH_CONTEXT(
        SDK, bool, ZEngineAppCommon_Initialize, ZEngineAppCommon* p_EngineAppCommon, const SRenderDestinationDesc& p_Description
    );
    DECLARE_THISCALL_DETOUR_WITH_CONTEXT(SDK, void, ZEngineAppCommon_Uninitialize, ZEngineAppCommon* p_EngineAppCommon);

    DECLARE_THISCALL_DETOUR_WITH_CONTEXT(SDK, void, ZRenderDevice_Present, ZRenderDevice* p_RenderDevice);
    DECLARE_THISCALL_DETOUR_WITH_CONTEXT(
        SDK, void, ZRenderSwapChain_Resize, ZRenderSwapChain* p_RenderSwapChain, const SRenderDestinationDesc* p_Description
    );

    std::shared_ptr<DirectXRenderer> m_DirectXRenderer;
    std::shared_ptr<ImGuiRenderer> m_ImGuiRenderer;

    std::shared_ptr<ModManager> m_ModManager;

    std::shared_ptr<UI::MainMenu> m_MainMenu;
    std::shared_ptr<UI::ModSelector> m_ModSelector;
    std::shared_ptr<UI::Console> m_Console;
    std::shared_ptr<UI::Settings> m_Settings;

    std::shared_ptr<ResourcePatcher> m_ResourcePatcher;

    uintptr_t m_ModuleBase;
    uint32_t m_SizeOfCode;
    uint32_t m_ImageSize;

#if _DEBUG
    std::shared_ptr<DebugConsole> m_DebugConsole{};
#endif
};
