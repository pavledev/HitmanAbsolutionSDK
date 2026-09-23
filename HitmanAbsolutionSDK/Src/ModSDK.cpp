#include <filesystem>
#include <Shlobj.h>

#include <MinHook.h>

#include <semver.hpp>

#include <ini.h>

#include <rapidjson/document.h>

#include <Glacier/ZModule.h>
#include <Glacier/ZApplication.h>
#include <Glacier/ZConfig.h>

#include "ModSDK.h"
#include "Globals.h"
#include "Logging.h"
#include "Hooks.h"
#include "IModInterface.h"
#include "Registry/ResourceIDRegistry.h"
#include "Registry/EnumRegistry.h"
#include "Registry/PropertyRegistry.h"
#include "Utils/ProcessUtils.h"
#include "UI/ModSelector.h"
#include "UI/MainMenu.h"
#include "UI/Console.h"
#include "HookImpl.h"
#include "DebugConsole.h"
#include "Rendering/DirectXRenderer.h"
#include "Rendering/ImGuiRenderer.h"
#include "Utils/ResourceUtils.h"
#include "Events.h"
#include "ResourcePatcher.h"

#include <winhttp.h>

// Needed for TaskDialogIndirect
#pragma comment(                                                                                                                                                               \
    linker,                                                                                                                                                                    \
    "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"" \
)

extern void SetupLogging(spdlog::level::level_enum p_LogLevel);
extern void FlushLoggers();
extern void ClearLoggers();
extern void DispatchLog(spdlog::level::level_enum p_Level, std::string_view p_Msg);

HitmanAbsolutionSDK_API IModSDK& SDK()
{
    return ModSDK::GetInstance();
}

extern "C" HitmanAbsolutionSDK_API const char* SDKVersion()
{
    return HMASDK_VERSION;
}

ModSDK::ModSDK()
{
    LoadConfiguration();

#if _DEBUG
    m_DebugConsole = std::make_shared<DebugConsole>();
    SetupLogging(spdlog::level::trace);
#else
    SetupLogging(spdlog::level::info);
#endif

    m_DirectXRenderer = std::make_shared<DirectXRenderer>();
    m_ImGuiRenderer = std::make_shared<ImGuiRenderer>();

    m_ModManager = std::make_shared<ModManager>();

    m_UIMainMenu = std::make_shared<UI::MainMenu>();
    m_UIModSelector = std::make_shared<UI::ModSelector>();
    m_UIConsole = std::make_shared<UI::Console>();

    if (m_EnableResourcePatching)
    {
        m_ResourcePatcher = std::make_shared<ResourcePatcher>();
    }

    HMODULE module = GetModuleHandleA(nullptr);

    m_ModuleBase = reinterpret_cast<uintptr_t>(module) + util::ProcessUtils::GetBaseOfCode(module);
    m_SizeOfCode = util::ProcessUtils::GetSizeOfCode(module);
    m_ImageSize = util::ProcessUtils::GetSizeOfImage(module);

    ResourceIDRegistry& resourceIDRegistry = ResourceIDRegistry::GetInstance();
    EnumRegistry& enumRegistry = EnumRegistry::GetInstance();
    PropertyRegistry& propertyRegistry = PropertyRegistry::GetInstance();
    std::thread thread = std::thread(&ResourceIDRegistry::Load, &resourceIDRegistry);
    std::thread thread2 = std::thread(&EnumRegistry::Load, &enumRegistry);
    std::thread thread3 = std::thread(&PropertyRegistry::Load, &propertyRegistry);

    thread.detach();
    thread2.detach();
    thread3.detach();
}

ModSDK& ModSDK::GetInstance()
{
    static ModSDK instance;

    return instance;
}

void ModSDK::Startup()
{
#if _DEBUG
    m_DebugConsole->StartRedirecting();
#endif

    Hooks::ZHitman5Module_Initialize->AddDetour(this, &ModSDK::ZHitman5Module_Initialize);
    Hooks::ZEngineAppCommon_Initialize->AddDetour(this, &ModSDK::ZEngineAppCommon_Initialize);
    Hooks::ZEngineAppCommon_Uninitialize->AddDetour(this, &ModSDK::ZEngineAppCommon_Uninitialize);

    Hooks::ZRenderDevice_Present->AddDetour(this, &ModSDK::ZRenderDevice_Present);
    Hooks::ZRenderSwapChain_Resize->AddDetour(this, &ModSDK::ZRenderSwapChain_Resize);

    auto result = MH_ApplyQueued();

    if (result != MH_OK)
    {
        Logger::Error("Could not enable hooks. Error code: {}.", static_cast<int>(result));
    }

    m_ModManager->LoadAllMods();

    result = MH_ApplyQueued();

    if (result != MH_OK)
    {
        Logger::Error("Could not enable hooks. Error code: {}.", static_cast<int>(result));
    }

    if (Globals::Hitman5Module->IsEngineInitialized())
    {
        OnEngineInitialized();
    }

    if (!m_DisableUpdateCheck)
    {
        std::thread versionCheckThread([&]() { CheckForUpdates(); });

        versionCheckThread.detach();
    }

    Events::OnConsoleCommand->AddListener(this, &OnConsoleCommand);
}

void ModSDK::Cleanup()
{
    // Close the hook gate and drain in-flight detours before tearing MinHook down.
    g_HookGate.BeginReload();

    const MH_STATUS status = MH_DisableHook(MH_ALL_HOOKS);

    if (status != MH_OK)
    {
        Logger::Error("Failed to disable hooks: {}.", static_cast<int>(status));
    }

    // Destroy renderer state while mods are still loaded.
    m_ImGuiRenderer->TeardownRenderer();
    m_DirectXRenderer->TeardownRenderer();

    m_ImGuiRenderer.reset();
    m_DirectXRenderer.reset();

    m_ModManager.reset();

    HookRegistry::ClearAllDetours();
    HookRegistry::DestroyHooks();

    if (MH_Uninitialize() != MH_OK)
    {
        Logger::Error("Failed to uninitialize MinHook.");
    }

    Trampolines::ClearTrampolines();
}

void ModSDK::OnEngineInitialized()
{
    m_ImGuiRenderer->OnEngineInitialized();
    m_DirectXRenderer->OnEngineInitialized();

    m_ModManager->LockRead();

    for (auto& loadedMod : m_ModManager->GetLoadedMods())
    {
        loadedMod->OnEngineInitialized();
    }

    m_ModManager->UnlockRead();
}

void ModSDK::OnEngineUninitialized()
{
    RequestCleanup();
}

void ModSDK::OnModLoaded(const std::string& p_Name, IModInterface* p_Mod, bool p_LiveLoad)
{
    p_Mod->SetupUI(m_ImGuiRenderer.get());
    p_Mod->Initialize();

    if (p_LiveLoad && Globals::Hitman5Module->IsEngineInitialized())
    {
        p_Mod->OnEngineInitialized();
    }

    Logger::Info("Mod {} successfully loaded.", p_Name);
}

void ModSDK::OnModUnloading(const std::string& p_Name, IModInterface* p_Mod)
{
    p_Mod->CleanupUI();
}

void ModSDK::OnModUnloaded(const std::string& p_Name) {}

void ModSDK::OnDrawMenu() const
{
    m_ModManager->LockRead();

    for (auto& mod : m_ModManager->GetLoadedMods())
    {
        mod->OnDrawMenu(m_ImGuiRenderer.get());
    }

    m_ModManager->UnlockRead();
}

void ModSDK::OnDrawUI(bool p_HasFocus) const
{
    m_UIConsole->Draw(m_ImGuiRenderer.get(), p_HasFocus);
    m_UIMainMenu->Draw(m_ImGuiRenderer.get(), p_HasFocus);
    m_UIModSelector->Draw(m_ImGuiRenderer.get(), p_HasFocus);

    m_ModManager->LockRead();

    for (auto& mod : m_ModManager->GetLoadedMods())
    {
        mod->OnDrawUI(m_ImGuiRenderer.get(), p_HasFocus);
    }

    m_ModManager->UnlockRead();
}

void ModSDK::OnDraw3D() const
{
    m_ModManager->LockRead();

    const bool isFrustumCullingEnabled = m_DirectXRenderer->IsFrustumCullingEnabled();
    const bool isDistanceCullingEnabled = m_DirectXRenderer->IsDistanceCullingEnabled();
    const float maxDrawDistance = m_DirectXRenderer->GetMaxDrawDistance();

    for (auto& mod : m_ModManager->GetLoadedMods())
    {
        mod->OnDraw3D(m_DirectXRenderer.get());

        m_DirectXRenderer->SetFrustumCullingEnabled(isFrustumCullingEnabled);
        m_DirectXRenderer->SetDistanceCullingEnabled(isDistanceCullingEnabled);
        m_DirectXRenderer->SetMaxDrawDistance(maxDrawDistance);
    }

    m_ModManager->UnlockRead();
}

void ModSDK::OnDepthDraw3D() const
{
    m_ModManager->LockRead();

    const bool isFrustumCullingEnabled = m_DirectXRenderer->IsFrustumCullingEnabled();
    const bool isDistanceCullingEnabled = m_DirectXRenderer->IsDistanceCullingEnabled();
    const float maxDrawDistance = m_DirectXRenderer->GetMaxDrawDistance();

    for (auto& mod : m_ModManager->GetLoadedMods())
    {
        mod->OnDepthDraw3D(m_DirectXRenderer.get());

        m_DirectXRenderer->SetFrustumCullingEnabled(isFrustumCullingEnabled);
        m_DirectXRenderer->SetDistanceCullingEnabled(isDistanceCullingEnabled);
        m_DirectXRenderer->SetMaxDrawDistance(maxDrawDistance);
    }

    m_ModManager->UnlockRead();
}

void ModSDK::OnDraw2D() const
{
    m_ModManager->LockRead();

    for (auto& mod : m_ModManager->GetLoadedMods())
    {
        mod->OnDraw2D(m_DirectXRenderer.get());
    }

    m_ModManager->UnlockRead();
}

void ModSDK::Log(spdlog::level::level_enum p_Level, std::string_view p_Msg)
{
    DispatchLog(p_Level, p_Msg);
}

bool ModSDK::PatchCode(const char* p_Pattern, const char* p_Mask, void* p_NewCode, size_t p_CodeSize, ptrdiff_t p_TargetOffset)
{
    return PatchCodeInternal(p_Pattern, p_Mask, p_NewCode, p_CodeSize, p_TargetOffset, nullptr);
}

bool ModSDK::PatchCodeStoreOriginal(
    const char* p_Pattern, const char* p_Mask, void* p_NewCode, size_t p_CodeSize, ptrdiff_t p_TargetOffset, void* p_OriginalCode
)
{
    return PatchCodeInternal(p_Pattern, p_Mask, p_NewCode, p_CodeSize, p_TargetOffset, p_OriginalCode);
}

void ModSDK::GetModSetting(IModInterface* p_Mod, const ZString& p_Section, const ZString& p_Name, const ZString& p_DefaultValue, ZString& p_OutValue)
{
    if (!p_Mod)
    {
        p_OutValue = p_DefaultValue;
        return;
    }

    auto settings = m_ModManager->GetModSettings(p_Mod);

    if (!settings)
    {
        p_OutValue = p_DefaultValue;
        return;
    }

    p_OutValue = settings->GetSetting(p_Section.ToCString(), p_Name.ToCString(), p_DefaultValue.ToCString());
}

int64_t ModSDK::GetModSettingInt(IModInterface* p_Mod, const ZString& p_Section, const ZString& p_Name, int64_t p_DefaultValue)
{
    if (!p_Mod)
    {
        return p_DefaultValue;
    }

    auto settings = m_ModManager->GetModSettings(p_Mod);

    if (!settings)
    {
        return p_DefaultValue;
    }

    const auto value = settings->GetSetting(p_Section.ToCString(), p_Name.ToCString(), std::to_string(p_DefaultValue));

    try
    {
        return std::stoll(value);
    }
    catch (const std::exception&)
    {
        return p_DefaultValue;
    }
}

uint64_t ModSDK::GetModSettingUInt(IModInterface* p_Mod, const ZString& p_Section, const ZString& p_Name, uint64_t p_DefaultValue)
{
    if (!p_Mod)
    {
        return p_DefaultValue;
    }

    auto settings = m_ModManager->GetModSettings(p_Mod);

    if (!settings)
    {
        return p_DefaultValue;
    }

    const auto value = settings->GetSetting(p_Section.ToCString(), p_Name.ToCString(), std::to_string(p_DefaultValue));

    try
    {
        return std::stoull(value);
    }
    catch (const std::exception&)
    {
        return p_DefaultValue;
    }
}

double ModSDK::GetModSettingDouble(IModInterface* p_Mod, const ZString& p_Section, const ZString& p_Name, double p_DefaultValue)
{
    if (!p_Mod)
    {
        return p_DefaultValue;
    }

    auto settings = m_ModManager->GetModSettings(p_Mod);

    if (!settings)
    {
        return p_DefaultValue;
    }

    const auto value = settings->GetSetting(p_Section.ToCString(), p_Name.ToCString(), std::to_string(p_DefaultValue));

    try
    {
        return std::stod(value);
    }
    catch (const std::exception&)
    {
        return p_DefaultValue;
    }
}

bool ModSDK::GetModSettingBool(IModInterface* p_Mod, const ZString& p_Section, const ZString& p_Name, bool p_DefaultValue)
{
    if (!p_Mod)
    {
        return p_DefaultValue;
    }

    auto settings = m_ModManager->GetModSettings(p_Mod);

    if (!settings)
    {
        return p_DefaultValue;
    }

    const auto value = settings->GetSetting(p_Section.ToCString(), p_Name.ToCString(), p_DefaultValue ? "true" : "false");

    if (value == "true" || value == "1" || value == "yes" || value == "on" || value == "y")
    {
        return true;
    }
    else if (value == "false" || value == "0" || value == "no" || value == "off" || value == "n")
    {
        return false;
    }
    else
    {
        return p_DefaultValue;
    }
}

void ModSDK::SetModSetting(IModInterface* p_Mod, const ZString& p_Section, const ZString& p_Name, const ZString& p_Value)
{
    if (!p_Mod)
    {
        return;
    }

    auto settings = m_ModManager->GetModSettings(p_Mod);

    if (!settings)
    {
        return;
    }

    settings->SetSetting(p_Section.ToCString(), p_Name.ToCString(), p_Value.ToCString());
}

void ModSDK::SetModSettingInt(IModInterface* p_Mod, const ZString& p_Section, const ZString& p_Name, int64_t p_Value)
{
    if (!p_Mod)
    {
        return;
    }

    auto settings = m_ModManager->GetModSettings(p_Mod);

    if (!settings)
    {
        return;
    }

    settings->SetSetting(p_Section.ToCString(), p_Name.ToCString(), std::to_string(p_Value));
}

void ModSDK::SetModSettingUInt(IModInterface* p_Mod, const ZString& p_Section, const ZString& p_Name, uint64_t p_Value)
{
    if (!p_Mod)
    {
        return;
    }

    auto settings = m_ModManager->GetModSettings(p_Mod);

    if (!settings)
    {
        return;
    }

    settings->SetSetting(p_Section.ToCString(), p_Name.ToCString(), std::to_string(p_Value));
}

void ModSDK::SetModSettingDouble(IModInterface* p_Mod, const ZString& p_Section, const ZString& p_Name, double p_Value)
{
    if (!p_Mod)
    {
        return;
    }

    auto settings = m_ModManager->GetModSettings(p_Mod);

    if (!settings)
    {
        return;
    }

    settings->SetSetting(p_Section.ToCString(), p_Name.ToCString(), std::to_string(p_Value));
}

void ModSDK::SetModSettingBool(IModInterface* p_Mod, const ZString& p_Section, const ZString& p_Name, bool p_Value)
{
    if (!p_Mod)
    {
        return;
    }

    auto settings = m_ModManager->GetModSettings(p_Mod);

    if (!settings)
    {
        return;
    }

    settings->SetSetting(p_Section.ToCString(), p_Name.ToCString(), p_Value ? "true" : "false");
}

bool ModSDK::HasModSetting(IModInterface* p_Mod, const ZString& p_Section, const ZString& p_Name)
{
    if (!p_Mod)
    {
        return false;
    }

    auto settings = m_ModManager->GetModSettings(p_Mod);

    if (!settings)
    {
        return false;
    }

    return settings->HasSetting(p_Section.ToCString(), p_Name.ToCString());
}

void ModSDK::RemoveModSetting(IModInterface* p_Mod, const ZString& p_Section, const ZString& p_Name)
{
    if (!p_Mod)
    {
        return;
    }

    auto settings = m_ModManager->GetModSettings(p_Mod);

    if (!settings)
    {
        return;
    }

    settings->RemoveSetting(p_Section.ToCString(), p_Name.ToCString());
}

void ModSDK::ReloadModSettings(IModInterface* p_Mod)
{
    if (!p_Mod)
    {
        return;
    }

    auto settings = m_ModManager->GetModSettings(p_Mod);

    if (!settings)
    {
        return;
    }

    settings->Reload();
}

bool ModSDK::WorldToScreen(const SVector3& p_WorldPos, SVector2& p_Out)
{
    return m_DirectXRenderer->WorldToScreen(p_WorldPos, p_Out);
}

bool ModSDK::ScreenToWorld(const SVector2& p_ScreenPos, SVector3& p_OutWorldPos, SVector3& p_OutDirection)
{
    return m_DirectXRenderer->ScreenToWorld(p_ScreenPos, p_OutWorldPos, p_OutDirection);
}

void ModSDK::RequestUIFocus()
{
    if (!m_UIEnabled)
    {
        return;
    }

    m_ImGuiRenderer.get()->SetFocus(true);
}

void ModSDK::ReleaseUIFocus()
{
    if (!m_UIEnabled)
    {
        return;
    }

    m_ImGuiRenderer.get()->SetFocus(false);
}

const char* ModSDK::GetResourceID(uint64_t p_RuntimeResourceID) const
{
    return ResourceIDRegistry::GetInstance().GetResourceID(p_RuntimeResourceID);
}

uint64_t ModSDK::GetRuntimeResourceID(const std::string& p_ResourceID) const
{
    ZRuntimeResourceID runtimeResourceID = ResourceIDRegistry::GetInstance().GetRuntimeResourceID(p_ResourceID);

    if (runtimeResourceID.GetID() == -1)
    {
        const std::string resourceID = util::ToLowerCase(p_ResourceID);

        runtimeResourceID = hash::GetMD5Hash64(resourceID);
    }

    if (Globals::LocalResourceResolver && !runtimeResourceID.IsLibraryResource())
    {
        (*Globals::LocalResourceResolver)->RecordMapping(runtimeResourceID, ZString(p_ResourceID));
    }

    return runtimeResourceID;
}

const std::map<int32_t, std::string>& ModSDK::GetEnum(const std::string& p_TypeName) const
{
    return EnumRegistry::GetInstance().GetEnum(p_TypeName);
}

const std::string& ModSDK::GetPropertyName(const uint32_t p_PropertyID) const
{
    return PropertyRegistry::GetInstance().GetPropertyName(p_PropertyID);
}

std::span<const std::byte> ModSDK::GetResource(int32_t p_ResourceID) const
{
    const HMODULE module = GetModuleHandleA("HitmanAbsolutionSDK.dll");

    if (!module)
    {
        return {};
    }

    const HRSRC resource = FindResource(module, MAKEINTRESOURCE(p_ResourceID), RT_RCDATA);

    if (!resource)
    {
        return {};
    }

    const HGLOBAL loadedResource = LoadResource(module, resource);

    if (!loadedResource)
    {
        return {};
    }

    const void* data = LockResource(loadedResource);

    if (!data)
    {
        return {};
    }

    return { static_cast<const std::byte*>(data), SizeofResource(module, resource) };
}

std::string_view ModSDK::GetTextResource(int32_t p_ResourceID) const
{
    const auto resource = GetResource(p_ResourceID);

    return { reinterpret_cast<const char*>(resource.data()), resource.size() };
}

bool ModSDK::InstallDynamicResourceLibrary(
    const ZRuntimeResourceID& p_HeaderLibraryRuntimeResourceID, ZDynamicResourceLibrary*& p_DynamicResourceLibrary,
    ZRuntimeResourceID& p_TempRuntimeResourceID, uint32_t p_EntityCount
) const
{
    return util::InstallDynamicResourceLibrary(p_HeaderLibraryRuntimeResourceID, p_DynamicResourceLibrary, p_TempRuntimeResourceID, p_EntityCount);
}

bool ModSDK::InstallDynamicResourceLibrary(
    const std::string& p_HeaderLibraryResourceID, ZDynamicResourceLibrary*& p_DynamicResourceLibrary, ZRuntimeResourceID& p_TempRuntimeResourceID,
    const uint32_t p_EntityCount
) const
{
    return util::InstallDynamicResourceLibrary(p_HeaderLibraryResourceID, p_DynamicResourceLibrary, p_TempRuntimeResourceID, p_EntityCount);
}

bool ModSDK::CreateAndInstallDynamicResourceLibrary(
    const std::string& p_ResourceID, ZDynamicResourceLibrary*& p_DynamicResourceLibrary, ZRuntimeResourceID& p_TempRuntimeResourceID,
    const uint32_t p_EntityCount
)
{
    return util::CreateAndInstallDynamicResourceLibrary(p_ResourceID, p_DynamicResourceLibrary, p_TempRuntimeResourceID, p_EntityCount);
}

void ModSDK::AllocateZString(ZString* p_Target, const char* p_Str, uint32_t p_Size)
{
    if (Globals::Hitman5Module->IsEngineInitialized())
    {
        // If engine is initialized, allocate the normal way.
        p_Target->m_length = p_Size;
        p_Target->m_chars = Functions::ZString_ZImpl_Allocate->Call(p_Str, p_Size)->m_pDataStart;
    }
    else
    {
        // Otherwise, allocate ourselves and make the game think it's a static allocation.
        // This will leak memory, but best we can do for now before the engine is initialized.
        auto* string = new char[p_Size + 1]{};
        memcpy(string, p_Str, p_Size);
        string[p_Size] = '\0';

        *p_Target = ZString(std::string_view(string, p_Size));
    }
}

void ModSDK::FreeZString(ZString* p_Target)
{
    if (p_Target->IsAllocated())
    {
        Functions::ZString_ZImpl_Free->Call(p_Target->GetImpl());
    }
}

bool ModSDK::PatchCodeInternal(
    const char* p_Pattern, const char* p_Mask, void* p_NewCode, size_t p_CodeSize, ptrdiff_t p_TargetOffset, void* p_OriginalCode
)
{
    if (!p_Pattern || !p_Mask || !p_NewCode || p_CodeSize == 0)
    {
        Logger::Error("Invalid parameters provided to PatchCode call.");
        return false;
    }

    const auto* pattern = reinterpret_cast<const uint8_t*>(p_Pattern);
    const auto target = util::ProcessUtils::SearchPattern(GetModuleBase(), GetSizeOfCode(), pattern, p_Mask);

    if (target == 0)
    {
        Logger::Error("Could not find pattern in call to PatchCode. Game might have been updated.");
        return false;
    }

    auto* targetPtr = reinterpret_cast<void*>(target + p_TargetOffset);

    if (p_OriginalCode != nullptr)
    {
        memcpy(p_OriginalCode, targetPtr, p_CodeSize);
    }

    Logger::Debug("Patching {} bytes of code at {} with new code from {}.", p_CodeSize, fmt::ptr(targetPtr), p_NewCode);

    DWORD oldProtect;
    VirtualProtect(targetPtr, p_CodeSize, PAGE_EXECUTE_READWRITE, &oldProtect);

    memcpy(targetPtr, p_NewCode, p_CodeSize);

    VirtualProtect(targetPtr, p_CodeSize, oldProtect, nullptr);

    return true;
}

void ModSDK::SetHasShownUIToggleWarning()
{
    m_HasShownUIToggleWarning.store(true, std::memory_order_release);

    SetConfigurationValue("ui", "shown_ui_toggle_warning", "true");
}

void ModSDK::LoadConfiguration()
{
    char exePathStr[MAX_PATH];
    const auto pathSize = GetModuleFileNameA(nullptr, exePathStr, MAX_PATH);

    if (pathSize == 0)
    {
        return;
    }

    const std::filesystem::path exePath(exePathStr);
    const auto iniPath = absolute(exePath.parent_path() / "sdk.ini");

    mINI::INIFile file(iniPath.string());
    mINI::INIStructure ini;

    if (!is_regular_file(iniPath))
    {
        return;
    }

    file.read(ini);

    const auto& general = ini["general"];

    if (general.has("read_hma_ini"))
    {
        m_ReadHMAIni = general.get("read_hma_ini") == "true";
    }

    if (general.has("enable_resource_patching"))
    {
        m_EnableResourcePatching = general.get("enable_resource_patching") == "true";
    }

    if (general.has("ignore_version"))
    {
        m_IgnoredVersion = general.get("ignore_version");
    }

    if (general.has("disable_update_check"))
    {
        m_DisableUpdateCheck = general.get("disable_update_check") == "true";
    }

    const auto& ui = ini["ui"];

    if (ui.has("noui") && ui.get("noui") == "true")
    {
        m_UIEnabled = false;

        MessageBoxA(
            nullptr,
            "WARNING: The mod SDK UI is currently disabled!\n\n"
            "If you want to re-enable it, set 'noui = false' in sdk.ini "
            "and restart your game.",
            "Mod SDK Warning", MB_OK | MB_ICONWARNING
        );
    }

    if (ui.has("console_key") && !ui.get("console_key").empty())
    {
        try
        {
            m_ConsoleScanCode = static_cast<uint8_t>(std::stoul(ui.get("console_key"), nullptr, 0));
        }
        catch (const std::exception&)
        {
            Logger::Error("Could not parse console_key value from sdk.ini. Using default value.");
        }
    }

    if (ui.has("ui_toggle_key") && !ui.get("ui_toggle_key").empty())
    {
        try
        {
            m_UIToggleScanCode = static_cast<uint8_t>(std::stoul(ui.get("ui_toggle_key"), nullptr, 0));
        }
        catch (const std::exception&)
        {
            Logger::Error("Could not parse ui_toggle_key value from sdk.ini. Using default value.");
        }
    }

    if (ui.has("shown_ui_toggle_warning"))
    {
        m_HasShownUIToggleWarning = true;
    }
}

void ModSDK::SetConfigurationValue(const std::string& p_Section, const std::string& p_Key, const std::string& p_Value)
{
    char exePathStr[MAX_PATH];
    const auto pathSize = GetModuleFileNameA(nullptr, exePathStr, MAX_PATH);

    if (pathSize == 0)
    {
        return;
    }

    const std::filesystem::path exePath(exePathStr);
    const auto exeDir = exePath.parent_path();
    const auto iniPath = absolute(exeDir / "sdk.ini");

    mINI::INIFile file(iniPath.string());
    mINI::INIStructure ini;

    if (is_regular_file(iniPath))
    {
        file.read(ini);
    }

    ini[p_Section].set(p_Key, p_Value);

    file.generate(ini, true);
}

std::pair<uint32_t, std::string> ModSDK::RequestLatestVersion()
{
    HINTERNET session =
        WinHttpOpen(L"HitmanAbsolutionSDK/1.0", WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);

    if (!session)
    {
        throw std::runtime_error("WinHttpOpen failed");
    }

    std::unique_ptr<void, decltype(&WinHttpCloseHandle)> sessionHandle(session, &WinHttpCloseHandle);

    HINTERNET connect = WinHttpConnect(sessionHandle.get(), L"api.github.com", 443, 0);

    if (!connect)
    {
        throw std::runtime_error("WinHttpConnect failed");
    }

    std::unique_ptr<void, decltype(&WinHttpCloseHandle)> connectHandle(connect, &WinHttpCloseHandle);

    HINTERNET request = WinHttpOpenRequest(
        connectHandle.get(), L"GET", L"/repos/pavledev/HitmanAbsolutionSDK/releases", nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES,
        WINHTTP_FLAG_SECURE
    );

    if (!request)
    {
        throw std::runtime_error("WinHttpOpenRequest failed");
    }

    std::unique_ptr<void, decltype(&WinHttpCloseHandle)> requestHandle(request, &WinHttpCloseHandle);

    if (!WinHttpSendRequest(requestHandle.get(), WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0))
    {
        throw std::runtime_error("WinHttpSendRequest failed");
    }

    if (!WinHttpReceiveResponse(requestHandle.get(), nullptr))
    {
        throw std::runtime_error("WinHttpReceiveResponse failed");
    }

    DWORD statusCode = 0;
    DWORD statusCodeSize = sizeof(statusCode);
    WinHttpQueryHeaders(
        requestHandle.get(), WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX, &statusCode, &statusCodeSize,
        WINHTTP_NO_HEADER_INDEX
    );

    DWORD responseSize = 0;
    std::string response;

    do
    {
        if (!WinHttpQueryDataAvailable(requestHandle.get(), &responseSize))
        {
            throw std::runtime_error("WinHttpQueryDataAvailable failed");
        }

        if (responseSize > 0)
        {
            // Read up to 128kb at a time.
            const auto sizeToRead = std::min<size_t>(responseSize, 128 * 1024);
            std::unique_ptr<char[]> buffer(new char[sizeToRead]);

            DWORD bytesRead;
            if (WinHttpReadData(requestHandle.get(), buffer.get(), sizeToRead, &bytesRead))
            {
                response.append(buffer.get(), bytesRead);
            }
        }
    }
    while (responseSize > 0);

    return { statusCode, response };
}

void ModSDK::ShowVersionNotice(const std::string& p_Version)
{
    const auto contentStr = std::format(
        "A new version of the Mod SDK ({}) is available.\nYou can update by downloading it and replacing "
        "the current version.\n\nKeep in mind you might also have to update any additional mods you have "
        "installed.",
        p_Version
    );

    const auto contentSize = MultiByteToWideChar(CP_UTF8, 0, contentStr.c_str(), -1, nullptr, 0);
    std::wstring wideContent(contentSize, 0);
    MultiByteToWideChar(CP_UTF8, 0, contentStr.c_str(), -1, wideContent.data(), contentSize);

    TASKDIALOGCONFIG config = { 0 };
    config.cbSize = sizeof(config);
    config.hInstance = GetModuleHandle(nullptr);
    config.dwFlags = TDF_ENABLE_HYPERLINKS | TDF_USE_COMMAND_LINKS;
    config.dwCommonButtons = TDCBF_CLOSE_BUTTON;
    config.pszMainIcon = TD_INFORMATION_ICON;
    config.pszWindowTitle = L"Hitman Absolution SDK - Update Available";
    config.pszMainInstruction = L"A new SDK update is available!";
    config.pszContent = wideContent.c_str();

    constexpr int downloadUpdateButtonID = 100;
    constexpr int viewUpdateNotesButtonID = 101;
    constexpr int ignoreVersionButtonID = 102;

    TASKDIALOG_BUTTON buttons[] = {
        { downloadUpdateButtonID, L"Download update\nUpdate must be manually applied after being downloaded." },
        { viewUpdateNotesButtonID, L"View update notes\nOpen a page with the update notes for the new version." },
        { ignoreVersionButtonID, L"Ignore this version\nSkip this update and further update notifications for this version." },
    };

    config.pButtons = buttons;
    config.cButtons = 3;

    struct TaskDialogCallbackData
    {
        std::string Version;
    };

    auto* callbackData = new TaskDialogCallbackData();
    callbackData->Version = p_Version;

    config.lpCallbackData = reinterpret_cast<LONG_PTR>(callbackData);
    config.pfCallback = [](HWND hWnd, UINT type, WPARAM wParam, LPARAM lParam, LONG_PTR data) -> HRESULT
    {
        auto* callbackData = reinterpret_cast<TaskDialogCallbackData*>(data);

        if (type == TDN_DESTROYED)
        {
            delete callbackData;
            return S_OK;
        }
        else if (type == TDN_BUTTON_CLICKED)
        {
            if (wParam == downloadUpdateButtonID)
            {
                const auto downloadURL = std::format(
                    "https://github.com/pavledev/HitmanAbsolutionSDK/releases/download/{}/HitmanAbsolutionSDK-Release.zip", callbackData->Version
                );

                ShellExecuteA(nullptr, "open", downloadURL.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
            }
            else if (wParam == viewUpdateNotesButtonID)
            {
                const auto releaseURL = std::format("https://github.com/pavledev/HitmanAbsolutionSDK/releases/tag/{}", callbackData->Version);

                ShellExecuteA(nullptr, "open", releaseURL.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
            }
            else if (wParam == ignoreVersionButtonID)
            {
                ModSDK::GetInstance().SkipVersionUpdate(callbackData->Version);
            }
        }

        return S_OK;
    };

    TaskDialogIndirect(&config, nullptr, nullptr, nullptr);
}

void ModSDK::SkipVersionUpdate(const std::string& p_Version)
{
    SetConfigurationValue("general", "ignore_version", p_Version);
}

bool ModSDK::CheckForUpdates() const
{
    if (m_DisableUpdateCheck)
    {
        Logger::Debug("Update check disabled. Skipping.");
        return false;
    }

    std::pair<uint32_t, std::string> versionCheckResult;

    try
    {
        Logger::Info("Checking for updates...");
        versionCheckResult = RequestLatestVersion();
    }
    catch (const std::exception& e)
    {
        Logger::Error("Could not check for updates: {}", e.what());
        return false;
    }

    if (versionCheckResult.first != 200)
    {
        Logger::Error("Could not check for updates: HTTP status code {}", versionCheckResult.first);
        return false;
    }

    rapidjson::Document document;

    if (document.Parse(versionCheckResult.second.data(), versionCheckResult.second.size()).HasParseError())
    {
        Logger::Error("Could not parse JSON response.");
        return false;
    }

    if (!document.IsArray() || document.Empty())
    {
        Logger::Error("Invalid JSON response: expected a non-empty array.");
        return false;
    }

    const auto& release = document[0];

    if (!release.IsObject() || !release.HasMember("tag_name") || !release["tag_name"].IsString())
    {
        Logger::Error("Invalid JSON response: missing or invalid tag_name.");
        return false;
    }

    const std::string_view latestVersion = release["tag_name"].GetString();

    if (m_IgnoredVersion == latestVersion)
    {
        Logger::Info("Ignoring update notification for version {}.", latestVersion);
        return false;
    }

    // Strip v prefix.
    const std::string latestVersionStr(latestVersion.substr(1));

    try
    {
        const auto currentVersion = semver::from_string(SDKVersion());
        const auto latestSemver = semver::from_string(latestVersionStr);

        if (latestSemver > currentVersion)
        {
            Logger::Info("A new version of the Mod SDK is available: {}.", latestVersion);
            ShowVersionNotice(std::string(latestVersion));
            return true;
        }

        Logger::Info("Mod SDK is up to date.");
    }
    catch (const std::exception& e)
    {
        Logger::Error("An error occurred while checking for updates: {}", e.what());
    }

    return false;
}

void ModSDK::OnConsoleCommand(void* p_Context, TArray<ZString>& p_Args)
{
    if (p_Args.Size() == 1)
    {
        if (p_Args[0] == "unloadall")
        {
            ModSDK::GetInstance().GetModManager()->UnloadAllMods();
        }
        else if (p_Args[0] == "reloadall")
        {
            ModSDK::GetInstance().GetModManager()->ReloadAllMods();
        }
    }

    if (p_Args.Size() == 2)
    {
        if (p_Args[0] == "load")
        {
            ModSDK::GetInstance().GetModManager()->LoadMod(p_Args[1].ToCString(), true);
        }
        else if (p_Args[0] == "unload")
        {
            ModSDK::GetInstance().GetModManager()->UnloadMod(p_Args[1].ToCString());
        }
        else if (p_Args[0] == "reload")
        {
            ModSDK::GetInstance().GetModManager()->ReloadMod(p_Args[1].ToCString());
        }
        else if (p_Args[0] == "config")
        {
            ZConfigCommand* configCommand = ZConfigCommand::GetConfigVariable(p_Args[1]);

            if (!configCommand)
            {
                Logger::Error("[ZConfigCommand] Invalid command.");
                return;
            }

            switch (configCommand->GetType())
            {
            case ZConfigCommand::ECLASSTYPE::ECLASS_FLOAT:
                Logger::Info("[ZConfigCommand] {} - float - {}", p_Args[1], configCommand->As<ZConfigFloat>()->m_Value);
                return;
            case ZConfigCommand::ECLASSTYPE::ECLASS_INT:
                Logger::Info("[ZConfigCommand] {} - int - {}", p_Args[1], configCommand->As<ZConfigInt>()->m_Value);
                return;
            case ZConfigCommand::ECLASSTYPE::ECLASS_STRING:
                Logger::Info("[ZConfigCommand] {} - string - \"{}\"", p_Args[1], configCommand->As<ZConfigString>()->m_szValue);
                return;
            case ZConfigCommand::ECLASSTYPE::ECLASS_UNKNOWN:
                Logger::Error("[ZConfigCommand] Unsupported command type (ECLASS_UNKNOWN).");
                return;
            }
        }
    }

    if (p_Args.Size() == 3)
    {
        if (p_Args[0] == "config")
        {
            ZConfigCommand* configCommand = ZConfigCommand::GetConfigVariable(p_Args[1]);

            if (!configCommand)
            {
                Logger::Info("[ZConfigCommand] Invalid command.");
                return;
            }

            switch (configCommand->GetType())
            {
            case ZConfigCommand::ECLASSTYPE::ECLASS_FLOAT: {
                try
                {
                    size_t parsedLength;
                    static_cast<void>(std::stof(p_Args[2].ToCString(), &parsedLength));

                    if (parsedLength != p_Args[2].Length())
                    {
                        Logger::Error("[ZConfigCommand] Invalid input (float), not all characters provided were processed.");
                        return;
                    }
                }
                catch (const std::invalid_argument&)
                {
                    Logger::Error("[ZConfigCommand] Invalid input (float), input does not represent a float.");
                    return;
                }
                catch (const std::out_of_range&)
                {
                    Logger::Error("[ZConfigCommand] Invalid input (float), float is out of range.");
                    return;
                }

                break;
            }
            case ZConfigCommand::ECLASSTYPE::ECLASS_INT: {
                try
                {
                    size_t parsedLength;
                    unsigned long value = std::stoul(p_Args[2].ToCString(), &parsedLength);

                    if (parsedLength != p_Args[2].Length())
                    {
                        Logger::Error("[ZConfigCommand] Invalid input (integer), not all characters provided were processed.");
                        return;
                    }

                    if (value > (std::numeric_limits<uint32_t>::max)())
                    {
                        Logger::Error("[ZConfigCommand] Invalid input (integer), out of u32 range.");
                        return;
                    }
                }
                catch (const std::invalid_argument&)
                {
                    Logger::Error("[ZConfigCommand] Invalid input (integer), input does not represent a integer.");
                    return;
                }
                catch (const std::out_of_range&)
                {
                    Logger::Error("[ZConfigCommand] Invalid input (integer), integer is out of range.");
                    return;
                }

                break;
            }
            case ZConfigCommand::ECLASSTYPE::ECLASS_STRING: {
                if (p_Args[2].Length() >= 256)
                {
                    return Logger::Error("[ZConfigCommand] Invalid input (string), maximum length of 255 exceeded.");
                }

                break;
            }
            case ZConfigCommand::ECLASSTYPE::ECLASS_UNKNOWN:
                Logger::Error("[ZConfigCommand] Unsupported command type (ECLASS_UNKNOWN).");
                return;
            }

            Functions::ZConfigCommand_ExecuteCommand->Call(p_Args[1].ToCString(), p_Args[2].ToCString());

            Logger::Info(R"([ZConfigCommand] Set "{}" to "{}")", p_Args[1], p_Args[2]);
        }
    }
}

DEFINE_THISCALL_DETOUR_WITH_CONTEXT(ModSDK, bool, ZHitman5Module_Initialize, ZHitman5Module* p_Hitman5Module)
{
    if (m_ReadHMAIni)
    {
        const std::filesystem::path iniFilePath = std::filesystem::current_path() / "HMA.ini";

        if (std::filesystem::exists(iniFilePath))
        {
            std::string iniFilePath2 = iniFilePath.string();

            std::replace(iniFilePath2.begin(), iniFilePath2.end(), '\\', '/');

            const ZFilePath filePath3 = ZFilePath(iniFilePath2.c_str());
            ZIniFile* iniFile = static_cast<ZIniFile*>((*Globals::ApplicationEngineWin32)->GetIniFile());
            TArray<unsigned char> buffer;

            ZIniFile::LoadIniFileContent(filePath3, buffer, true);

            if (buffer.Size() > 0)
            {
                const ZString fileContent = ZString(reinterpret_cast<char*>(buffer.GetStart()));

                iniFile->LoadFromStringInternal(fileContent, filePath3);

                int argc = 0;

                (*Globals::ApplicationEngineWin32)->AddApplicationSpecificOptions(iniFile);
                (*Globals::ApplicationEngineWin32)->ApplyOptionOverrides(argc, nullptr);
            }

            Logger::Info("HMA.ini was read successfully.");
        }
    }

    return { HookAction::Continue() };
}

DEFINE_THISCALL_DETOUR_WITH_CONTEXT(
    ModSDK, bool, ZEngineAppCommon_Initialize, ZEngineAppCommon* p_EngineAppCommon, const SRenderDestinationDesc& p_Description
)
{
    bool result = p_Hook->CallOriginal(p_EngineAppCommon, p_Description);

    OnEngineInitialized();

    return { HookAction::Return(), result };
}

DEFINE_THISCALL_DETOUR_WITH_CONTEXT(ModSDK, void, ZEngineAppCommon_Uninitialize, ZEngineAppCommon* p_EngineAppCommon)
{
    p_Hook->CallOriginal(p_EngineAppCommon);

    OnEngineUninitialized();

    return { HookAction::Return() };
}

DEFINE_THISCALL_DETOUR_WITH_CONTEXT(ModSDK, void, ZRenderDevice_Present, ZRenderDevice* p_RenderDevice)
{
    m_DirectXRenderer->OnPresent(p_RenderDevice);
    m_ImGuiRenderer->OnPresent(p_RenderDevice);

    return { HookAction::Continue() };
}

DEFINE_THISCALL_DETOUR_WITH_CONTEXT(
    ModSDK, void, ZRenderSwapChain_Resize, ZRenderSwapChain* p_RenderSwapChain, const SRenderDestinationDesc* p_Description
)
{
    m_DirectXRenderer->OnResize(p_Description);
    m_ImGuiRenderer->OnResize(p_Description);

    return { HookAction::Continue() };
}
