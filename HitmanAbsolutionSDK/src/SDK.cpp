#include <filesystem>

#include <MinHook.h>

#include <Glacier/ZModule.h>
#include <Glacier/ZApplication.h>

#include "SDK.h"
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
#include "UI/Settings.h"
#include "HookImpl.h"
#include "DebugConsole.h"
#include "Renderer/DirectXRenderer.h"
#include "Renderer/ImGuiRenderer.h"
#include "Utils/ResourceUtils.h"

extern void SetupLogging(spdlog::level::level_enum logLevel);

SDK::SDK()
{
#if _DEBUG
    m_DebugConsole = std::make_shared<DebugConsole>();
    SetupLogging(spdlog::level::trace);
#else
    SetupLogging(spdlog::level::info);
#endif

    m_DirectXRenderer = std::make_shared<DirectXRenderer>();
    m_ImGuiRenderer = std::make_shared<ImGuiRenderer>();

    m_ModManager = std::make_shared<ModManager>();

    m_MainMenu = std::make_shared<UI::MainMenu>();
    m_ModSelector = std::make_shared<UI::ModSelector>();
    m_Settings = std::make_shared<UI::Settings>();
    m_Console = std::make_shared<UI::Console>();

    /*if (settings->PatchResources())
    {
        resourcePatcher = std::make_shared<ResourcePatcher>();
    }*/

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

SDK& SDK::GetInstance()
{
    static SDK instance;

    return instance;
}

void SDK::Setup()
{
#if _DEBUG
    m_DebugConsole->StartRedirecting();
#endif

    Hooks::ZHitman5Module_Initialize->AddDetour(this, &SDK::ZHitman5Module_Initialize);
    Hooks::ZEngineAppCommon_Initialize->AddDetour(this, &SDK::ZEngineAppCommon_Initialize);
    Hooks::ZEngineAppCommon_Uninitialize->AddDetour(this, &SDK::ZEngineAppCommon_Uninitialize);

    Hooks::ZRenderDevice_Present->AddDetour(this, &SDK::ZRenderDevice_Present);
    Hooks::ZRenderSwapChain_Resize->AddDetour(this, &SDK::ZRenderSwapChain_Resize);

    m_ModManager->LoadAllMods();

    if (Globals::Hitman5Module->IsEngineInitialized())
    {
        OnEngineInitialized();
    }

    const MH_STATUS status = MH_EnableHook(MH_ALL_HOOKS);

    if (status != MH_OK)
    {
        Logger::Error("Failed to enable hooks. MinHook error: {}.", static_cast<int>(status));
    }
}

void SDK::Cleanup()
{
    m_ModManager.reset();

    HookRegistry::ClearDetoursWithContext(this);

    m_DirectXRenderer->Cleanup();
    m_ImGuiRenderer->Cleanup();

    HookRegistry::DestroyHooks();
    Trampolines::ClearTrampolines();

    if (MH_Uninitialize() != MH_OK)
    {
        Logger::Error("Failed to uninitialize MinHook.");
    }
}

void SDK::OnEngineInitialized()
{
    m_ImGuiRenderer->OnEngineInitialized();

    m_ModManager->LockRead();

    for (auto& loadedMod : m_ModManager->GetLoadedMods())
    {
        loadedMod.second.m_ModInterface->OnEngineInitialized();
    }

    m_ModManager->UnlockRead();
}

void SDK::OnEngineUninitialized()
{
    Cleanup();
}

void SDK::OnModLoaded(const std::string& p_Name, IModInterface* m_ModInterface, const bool m_LiveLoad)
{
    m_ModInterface->SetupUI();
    m_ModInterface->Initialize();
    m_ModInterface->LoadConfiguration(p_Name);

    if (m_LiveLoad && Globals::Hitman5Module->IsEngineInitialized())
    {
        m_ModInterface->OnEngineInitialized();
    }
}

void SDK::OnDrawUI(const bool hasFocus)
{
    m_MainMenu->Draw(hasFocus);
    m_ModSelector->Draw(hasFocus);
    m_Settings->Draw(hasFocus);
    m_Console->Draw(hasFocus);

    m_ModManager->LockRead();

    for (auto& mod : m_ModManager->GetLoadedMods())
    {
        mod.second.m_ModInterface->OnDrawUI(hasFocus);
    }

    m_ModManager->UnlockRead();
}

void SDK::OnDraw3D()
{
    m_ModManager->LockRead();

    for (auto& mod : m_ModManager->GetLoadedMods())
    {
        mod.second.m_ModInterface->OnDraw3D();
    }

    m_ModManager->UnlockRead();
}

void SDK::OnDrawMenu()
{
    m_ModManager->LockRead();

    for (auto& mod : m_ModManager->GetLoadedMods())
    {
        mod.second.m_ModInterface->OnDrawMenu();
    }

    m_ModManager->UnlockRead();
}

const char* SDK::GetResourceID(uint64_t p_RuntimeResourceID) const
{
    return ResourceIDRegistry::GetInstance().GetResourceID(p_RuntimeResourceID);
}

uint64_t SDK::GetRuntimeResourceID(const std::string& p_ResourceID) const
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

const std::map<int, std::string>& SDK::GetEnum(const std::string& p_TypeName)
{
    return EnumRegistry::GetInstance().GetEnum(p_TypeName);
}

const std::string& SDK::GetPropertyName(const uint32_t p_PropertyID) const
{
    return PropertyRegistry::GetInstance().GetPropertyName(p_PropertyID);
}

bool SDK::CreateAndInstallDynamicResourceLibrary(
    const std::string& p_ResourceID, ZDynamicResourceLibrary*& p_DynamicResourceLibrary, ZRuntimeResourceID& p_TempRuntimeResourceID,
    const uint32_t p_EntityCount
)
{
    return util::CreateAndInstallDynamicResourceLibrary(p_ResourceID, p_DynamicResourceLibrary, p_TempRuntimeResourceID, p_EntityCount);
}

ImGuiContext* SDK::GetImGuiContext()
{
    return m_ImGuiRenderer->GetImGuiContext();
}

ImGuiMemAllocFunc SDK::GetImGuiMemAllocFunc()
{
    return m_ImGuiRenderer->GetImGuiMemAllocFunc();
}

ImGuiMemFreeFunc SDK::GetImGuiMemFreeFunc()
{
    return m_ImGuiRenderer->GetImGuiMemFreeFunc();
}

void* SDK::GetImGuiUserDataAllocator()
{
    return m_ImGuiRenderer->GetImGuiUserDataAllocator();
}

ImFont* SDK::GetRegularFont()
{
    return m_ImGuiRenderer->GetRegularFont();
}

ImFont* SDK::GetBoldFont()
{
    return m_ImGuiRenderer->GetBoldFont();
}

std::shared_ptr<DirectXRenderer> SDK::GetDirectXRenderer() const
{
    return m_DirectXRenderer;
}

std::shared_ptr<ImGuiRenderer> SDK::GetImGuiRenderer() const
{
    return m_ImGuiRenderer;
}

std::shared_ptr<ModManager> SDK::GetModManager() const
{
    return m_ModManager;
}

std::shared_ptr<UI::ModSelector> SDK::GetModSelector() const
{
    return m_ModSelector;
}

std::shared_ptr<UI::Console> SDK::GetConsole() const
{
    return m_Console;
}

std::shared_ptr<UI::Settings> SDK::GetSettings() const
{
    return m_Settings;
}

std::shared_ptr<ResourcePatcher> SDK::GetResourcePatcher() const
{
    return m_ResourcePatcher;
}

DEFINE_THISCALL_DETOUR_WITH_CONTEXT(SDK, bool, ZHitman5Module_Initialize, ZHitman5Module* p_Hitman5Module)
{
    std::shared_ptr<UI::Settings> settings = SDK::GetInstance().GetSettings();

    if (settings->ReadHMAIni())
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

    bool result = p_Hook->CallOriginal(p_Hitman5Module);

    // SDK::GetInstance().OnEngineInitialized();

    if (settings->IniFileHasKey("Settings", "PauseOnFocusLoss"))
    {
        const char* value = settings->PauseOnFocusLoss() ? "true" : "false";

        (*Globals::ApplicationEngineWin32)->SetOption("PauseOnFocusLoss", value);
    }
    else
    {
        const bool pauseOnFocusLoss = Functions::GetApplicationOptionBool->Call("PauseOnFocusLoss", false);

        settings->SetPauseOnFocusLoss(pauseOnFocusLoss);
    }

    if (settings->IniFileHasKey("Settings", "MinimizeOnFocusLoss"))
    {
        const char* value = settings->MinimizeOnFocusLoss() ? "true" : "false";

        (*Globals::ApplicationEngineWin32)->SetOption("NO_MINIMIZE_FOCUSLOSS", value);
    }
    else
    {
        const bool minimizeOnFocusLoss = Functions::GetApplicationOptionBool->Call("NO_MINIMIZE_FOCUSLOSS", false);

        settings->SetMinimizeOnFocusLoss(minimizeOnFocusLoss);
    }

    return { HookAction::Return(), result };
}

DEFINE_THISCALL_DETOUR_WITH_CONTEXT(
    SDK, bool, ZEngineAppCommon_Initialize, ZEngineAppCommon* p_EngineAppCommon, const SRenderDestinationDesc& p_Description
)
{
    bool result = p_Hook->CallOriginal(p_EngineAppCommon, p_Description);

    OnEngineInitialized();

    return { HookAction::Return(), result };
}

DEFINE_THISCALL_DETOUR_WITH_CONTEXT(SDK, void, ZEngineAppCommon_Uninitialize, ZEngineAppCommon* p_EngineAppCommon)
{
    p_Hook->CallOriginal(p_EngineAppCommon);

    SDK::GetInstance().OnEngineUninitialized();

    return { HookAction::Return() };
}

DEFINE_THISCALL_DETOUR_WITH_CONTEXT(SDK, void, ZRenderDevice_Present, ZRenderDevice* p_RenderDevice)
{
    m_DirectXRenderer->OnPresent(p_RenderDevice);
    m_ImGuiRenderer->OnPresent(p_RenderDevice);

    return { HookAction::Continue() };
}

DEFINE_THISCALL_DETOUR_WITH_CONTEXT(
    SDK, void, ZRenderSwapChain_Resize, ZRenderSwapChain* p_RenderSwapChain, const SRenderDestinationDesc* p_Description
)
{
    m_DirectXRenderer->OnResize(p_Description);
    m_ImGuiRenderer->OnResize(p_Description);

    return { HookAction::Continue() };
}
