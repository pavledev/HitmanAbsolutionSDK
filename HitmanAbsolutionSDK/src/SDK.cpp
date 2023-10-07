#include <filesystem>

#include <MinHook.h>

#include <Glacier/Module/ZHitman5Module.h>
#include <Glacier/Engine/ZApplicationEngineWin32.h>
#include <Glacier/Engine/ZIniFile.h>

#include "SDK.h"
#include "Global.h"
#include "Function.h"
#include "Logger.h"
#include "Hooks.h"
#include "ModInterface.h"

uintptr_t BaseAddress;
ZRenderManager* RenderManager;
ZLevelManager* LevelManager;
ZGraphicsSettingsManager* GraphicsSettingsManager;
ZMemoryManager* MemoryManager;
ZGameTimeManager* GameTimeManager;
ZInputDeviceManagerWindows* InputDeviceManager;
ZInputActionManager* InputActionManager;
ZHitman5Module* Hitman5Module;
ZGameLoopManager* GameLoopManager;
ZGameWideUI* GameWideUI;
ZHUDManager* HUDManager;
ZScaleformManager* ScaleformManager;
ZInputAction* HM5InputControl;
ZCollisionManager* CollisionManager;
ZTypeRegistry** TypeRegistry;
bool IsEngineInitialized;

SDK::SDK()
{
    InitializeSingletons();

    if (MH_Initialize() != MH_OK)
    {
        Logger::GetInstance().Log(Logger::Level::Error, "Failed to initialize MinHook!");
    }

    directxRenderer = std::make_shared<DirectXRenderer>();
    imGuiRenderer = std::make_shared<ImGuiRenderer>();

    modManager = std::make_shared<ModManager>();

    mainMenu = std::make_shared<MainMenu>();
    modSelector = std::make_shared<ModSelector>();

    
    Hooks::ZRenderDevice_PresentHook.CreateHook("ZRenderDevice::Present", 0x5A7F30, ZRenderDevice_PresentHook);
    Hooks::ZRenderSwapChain_ResizeHook.CreateHook("ZRenderSwapChain::Resize", 0x2FA520, ZRenderSwapChain_ResizeHook);
    Hooks::ZApplicationEngineWin32_MainWindowProc.CreateHook("ZApplicationEngineWin32::MainWindowProc", 0x4FF520, ZApplicationEngineWin32_MainWindowProcHook);
    Hooks::ZHitman5Module_Initialize.CreateHook("ZHitman5Module::Initialize", 0x58E8D0, ZHitman5Module_InitializeHook);
    Hooks::ZEngineAppCommon_Initialize.CreateHook("ZEngineAppCommon::Initialize", 0x55A620, ZEngineAppCommon_InitializeHook);
    Hooks::ZEngineAppCommon_UpdateInputDeviceManager.CreateHook("ZEngineAppCommon::UpdateInputDeviceManager", 0x46C1D0, ZEngineAppCommon_UpdateInputDeviceManagerHook);

    Hooks::ZRenderDevice_PresentHook.EnableHook();
    Hooks::ZRenderSwapChain_ResizeHook.EnableHook();
    Hooks::ZApplicationEngineWin32_MainWindowProc.EnableHook();
    Hooks::ZHitman5Module_Initialize.EnableHook();
    Hooks::ZEngineAppCommon_Initialize.EnableHook();
    Hooks::ZEngineAppCommon_UpdateInputDeviceManager.EnableHook();
}

SDK::~SDK()
{
    Hooks::ZRenderDevice_PresentHook.DisableHook();
    Hooks::ZRenderDevice_PresentHook.RemoveHook();

    Hooks::ZRenderSwapChain_ResizeHook.DisableHook();
    Hooks::ZRenderSwapChain_ResizeHook.RemoveHook();

    Hooks::ZApplicationEngineWin32_MainWindowProc.DisableHook();
    Hooks::ZApplicationEngineWin32_MainWindowProc.RemoveHook();
}

SDK& SDK::GetInstance()
{
    static SDK instance;

    return instance;
}

void SDK::Setup()
{
    modManager->LoadAllMods();

    /*modManager->LockRead();

    for (const auto& mod : modManager->GetLoadedMods())
    {
        mod.second.modInterface->SetupUI();
        mod.second.modInterface->Initialize();
    }

    modManager->UnlockRead();*/

    if (IsEngineInitialized)
    {
        OnEngineInitialized();
    }
}

void SDK::Cleanup()
{
    directxRenderer->Cleanup();
    imGuiRenderer->Cleanup();

    if (MH_DisableHook(MH_ALL_HOOKS) != MH_OK)
    {
        Logger::GetInstance().Log(Logger::Level::Error, "Failed to disable hooks.");
    }

    if (MH_Uninitialize() != MH_OK)
    {
        Logger::GetInstance().Log(Logger::Level::Error, "Failed to uninitialize MinHook.");
    }
}

ZMemoryManager* SDK::GetMemoryManager()
{
    return Function::CallAndReturn<ZMemoryManager*>(BaseAddress + 0x389F10);
}

void SDK::InitializeSingletons()
{
    BaseAddress = reinterpret_cast<uintptr_t>(GetModuleHandleA(nullptr));
    RenderManager = reinterpret_cast<ZRenderManager*>(BaseAddress + 0xE31B80);
    LevelManager = reinterpret_cast<ZLevelManager*>(BaseAddress + 0xE21310);
    GraphicsSettingsManager = reinterpret_cast<ZGraphicsSettingsManager*>(BaseAddress + 0xD57190);
    MemoryManager = GetMemoryManager();
    GameTimeManager = reinterpret_cast<ZGameTimeManager*>(BaseAddress + 0xE24730);
    InputDeviceManager = reinterpret_cast<ZInputDeviceManagerWindows*>(BaseAddress + 0xE2EE10);
    InputActionManager = reinterpret_cast<ZInputActionManager*>(BaseAddress + 0xE2F7D0);
    Hitman5Module = reinterpret_cast<ZHitman5Module*>(BaseAddress + 0xE21B30);
    GameLoopManager = reinterpret_cast<ZGameLoopManager*>(BaseAddress + 0xE24630);
    GameWideUI = reinterpret_cast<ZGameWideUI*>(BaseAddress + 0xE21690);
    HUDManager = reinterpret_cast<ZHUDManager*>(BaseAddress + 0xD61C00);
    ScaleformManager = reinterpret_cast<ZScaleformManager*>(BaseAddress + 0xE550F0);
    HM5InputControl = reinterpret_cast<ZInputAction*>(BaseAddress + 0xD4DE98);
    CollisionManager = reinterpret_cast<ZCollisionManager*>(BaseAddress + 0xE54440);
    TypeRegistry = reinterpret_cast<ZTypeRegistry**>(BaseAddress + 0xD47BFC);

    ZApplicationEngineWin32::SetInstance(reinterpret_cast<ZApplicationEngineWin32**>(BaseAddress + 0xCC6B90));
}

void SDK::OnEngineInitialized()
{
    modManager->LockRead();

    for (auto& loadedMod : modManager->GetLoadedMods())
    {
        loadedMod.second.modInterface->OnEngineInitialized();
    }

    modManager->UnlockRead();
}

void SDK::OnModLoaded(const std::string& name, ModInterface* modInterface, const bool liveLoad)
{
    modInterface->SetupUI();
    modInterface->Initialize();

    if (liveLoad && IsEngineInitialized)
    {
        modInterface->OnEngineInitialized();
    }
}

void SDK::OnDrawUI(const bool hasFocus)
{
    mainMenu->Draw(hasFocus);
    modSelector->Draw(hasFocus);

    modManager->LockRead();

    for (auto& mod : modManager->GetLoadedMods())
    {
        mod.second.modInterface->OnDrawUI(hasFocus);
    }

    modManager->UnlockRead();
}

void SDK::OnDraw3D()
{
    modManager->LockRead();

    for (auto& mod : modManager->GetLoadedMods())
    {
        mod.second.modInterface->OnDraw3D();
    }

    modManager->UnlockRead();
}

void SDK::OnDrawMenu()
{
    modManager->LockRead();

    for (auto& mod : modManager->GetLoadedMods())
    {
        mod.second.modInterface->OnDrawMenu();
    }

    modManager->UnlockRead();
}

void SDK::OnPresent(ZRenderDevice* renderDevice)
{
    directxRenderer->OnPresent(renderDevice);
    imGuiRenderer->OnPresent(renderDevice);
}

void SDK::OnResize(const SRenderDestinationDesc* pDescription)
{
    directxRenderer->OnResize(pDescription);
}

long SDK::MainWindowProc(ZApplicationEngineWin32* applicationEngineWin32, HWND hWnd, unsigned int uMsgId, unsigned int wParam, long lParam)
{
    return imGuiRenderer->MainWindowProc(applicationEngineWin32, hWnd, uMsgId, wParam, lParam);
}

void SDK::OnUpdateInputDeviceManager(ZEngineAppCommon* engineAppCommon)
{
    imGuiRenderer->OnUpdateInputDeviceManager(engineAppCommon);
}

ImGuiContext* SDK::GetImGuiContext()
{
    return imGuiRenderer->GetImGuiContext();
}

ImGuiMemAllocFunc SDK::GetImGuiMemAllocFunc()
{
    return imGuiRenderer->GetImGuiMemAllocFunc();
}

ImGuiMemFreeFunc SDK::GetImGuiMemFreeFunc()
{
    return imGuiRenderer->GetImGuiMemFreeFunc();
}

void* SDK::GetImGuiUserDataAllocator()
{
    return imGuiRenderer->GetImGuiUserDataAllocator();
}

ImFont* SDK::GetRegularFont()
{
    return imGuiRenderer->GetRegularFont();
}

ImFont* SDK::GetBoldFont()
{
    return imGuiRenderer->GetBoldFont();
}

std::shared_ptr<ModManager> SDK::GetModManager()
{
    return modManager;
}

std::shared_ptr<ModSelector> SDK::GetModSelector()
{
    return modSelector;
}

void __fastcall ZRenderDevice_PresentHook(ZRenderDevice* pThis, int edx)
{
    SDK::GetInstance().OnPresent(pThis);

    Hooks::ZRenderDevice_PresentHook.CallOriginalFunction(pThis);
}

void __fastcall ZRenderSwapChain_ResizeHook(ZRenderSwapChain* pThis, int edx, const SRenderDestinationDesc* pDescription)
{
    SDK::GetInstance().OnResize(pDescription);

    Hooks::ZRenderSwapChain_ResizeHook.CallOriginalFunction(pThis, pDescription);
}

long __stdcall ZApplicationEngineWin32_MainWindowProcHook(ZApplicationEngineWin32* pThis, HWND hWnd, unsigned int uMsgId, unsigned int wParam, long lParam)
{
    return SDK::GetInstance().MainWindowProc(pThis, hWnd, uMsgId, wParam, lParam);
}

bool __fastcall ZHitman5Module_InitializeHook(ZHitman5Module* pThis, int edx)
{
    std::filesystem::path iniFilePath = std::filesystem::current_path() / "HMA.ini";

    if (std::filesystem::exists(iniFilePath))
    {
        std::string iniFilePath2 = iniFilePath.string();

        std::replace(iniFilePath2.begin(), iniFilePath2.end(), '\\', '/');

        ZFilePath filePath3 = ZFilePath(iniFilePath2.c_str());
        ZIniFile* iniFile = static_cast<ZIniFile*>(ZApplicationEngineWin32::GetInstance()->GetIniFile());
        TArray<unsigned char> buffer;

        ZIniFile::LoadIniFileContent(filePath3, buffer, true);

        if (buffer.Size() > 0)
        {
            ZString fileContent = ZString(reinterpret_cast<char*>(buffer.GetStart()));

            iniFile->LoadFromStringInternal(fileContent, filePath3);

            int argc = 0;

            ZApplicationEngineWin32::GetInstance()->AddApplicationSpecificOptions(iniFile);
            ZApplicationEngineWin32::GetInstance()->ApplyOptionOverrides(argc, nullptr);
        }
    }

    bool result = Hooks::ZHitman5Module_Initialize.CallOriginalFunction(pThis);

    //SDK::GetInstance().OnEngineInitialized();

    return result;
}

bool __fastcall ZEngineAppCommon_InitializeHook(ZEngineAppCommon* pThis, int edx, const SRenderDestinationDesc& description)
{
    IsEngineInitialized = Hooks::ZEngineAppCommon_Initialize.CallOriginalFunction(pThis, description);

    SDK::GetInstance().OnEngineInitialized();

    return IsEngineInitialized;
}

void __fastcall ZEngineAppCommon_UpdateInputDeviceManagerHook(ZEngineAppCommon* pThis, int edx)
{
    SDK::GetInstance().OnUpdateInputDeviceManager(pThis);
}
