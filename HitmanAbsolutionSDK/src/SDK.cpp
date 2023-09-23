#include <MinHook.h>

#include <Glacier/Module/ZHitman5Module.h>

#include "SDK.h"
#include "Global.h"
#include "Function.h"
#include "Logger.h"
#include "Hooks.h"

SDK::SDK()
{
    if (MH_Initialize() != MH_OK)
    {
        Logger::GetInstance().Log(Logger::Level::Error, "Failed to initialize MinHook.");
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

    Hooks::ZRenderDevice_PresentHook.EnableHook();
    Hooks::ZRenderSwapChain_ResizeHook.EnableHook();
    Hooks::ZApplicationEngineWin32_MainWindowProc.EnableHook();
    Hooks::ZHitman5Module_Initialize.EnableHook();
}

SDK::~SDK()
{
    Hooks::ZRenderDevice_PresentHook.DisableHook();
    Hooks::ZRenderSwapChain_ResizeHook.DisableHook();

    Hooks::ZRenderDevice_PresentHook.DisableHook();
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
    InitializeSingletons();
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
    const uintptr_t baseAddress = reinterpret_cast<uintptr_t>(GetModuleHandleA(nullptr));

    return Function::CallAndReturn<ZMemoryManager*>(baseAddress + 0x389F10);
}

void SDK::InitializeSingletons()
{
    const uintptr_t baseAddress = reinterpret_cast<uintptr_t>(GetModuleHandleA(nullptr));

    renderManager = reinterpret_cast<ZRenderManager*>(baseAddress + 0xE31B80);
    levelManager = reinterpret_cast<ZLevelManager*>(baseAddress + 0xE21310);
    graphicsSettingsManager = reinterpret_cast<ZGraphicsSettingsManager*>(baseAddress + 0xD57190);
    memoryManager = GetMemoryManager();
    gameTimeManager = reinterpret_cast<ZGameTimeManager*>(baseAddress + 0xE24730);
    inputDeviceManager = reinterpret_cast<ZInputDeviceManagerWindows*>(baseAddress + 0xE2EE10);
    inputActionManager = reinterpret_cast<ZInputActionManager*>(baseAddress + 0xE2F7D0);
    hitman5Module = reinterpret_cast<ZHitman5Module*>(baseAddress + 0xE21B30);
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

    if (liveLoad && hitman5Module->IsEngineInitialized())
    {
        modInterface->OnEngineInitialized();
    }
}

void SDK::OnDrawUI(const bool hasFocus)
{
    mainMenu->Draw(hasFocus);
    modSelector->Draw(hasFocus);
}

void SDK::OnDraw3D()
{
}

void SDK::OnDrawMenu()
{
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
    bool result = Hooks::ZHitman5Module_Initialize.CallOriginalFunction(pThis);

    SDK::GetInstance().OnEngineInitialized();

    return result;
}
