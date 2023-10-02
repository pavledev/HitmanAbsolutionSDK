#pragma once

#include "Renderer/DirectXRenderer.h"
#include "Renderer/ImGuiRenderer.h"
#include "ModManager.h"
#include "UI/MainMenu.h"
#include "UI/ModSelector.h"

class ZMemoryManager;
class ZHitman5Module;
class ZEngineAppCommon;

void __fastcall ZRenderDevice_PresentHook(ZRenderDevice* pThis, int edx);
void __fastcall ZRenderSwapChain_ResizeHook(ZRenderSwapChain* pThis, int edx, const SRenderDestinationDesc* pDescription);
long __stdcall ZApplicationEngineWin32_MainWindowProcHook(ZApplicationEngineWin32* pThis, HWND hWnd, unsigned int uMsgId, unsigned int wParam, long lParam);
bool __fastcall ZHitman5Module_InitializeHook(ZHitman5Module* pThis, int edx);
bool __fastcall ZEngineAppCommon_InitializeHook(ZEngineAppCommon* pThis, int edx, const SRenderDestinationDesc& description);

class SDK
{
public:
	~SDK();
	HitmanAbsolutionSDK_API static SDK& GetInstance();
	void Setup();
	void Cleanup();

	static ZMemoryManager* GetMemoryManager();
	static void InitializeSingletons();

	void OnEngineInitialized();
	void OnModLoaded(const std::string& name, ModInterface* modInterface, const bool liveLoad);
	void OnDrawUI(const bool hasFocus);
	void OnDraw3D();
	void OnDrawMenu();

	void OnPresent(ZRenderDevice* renderDevice);
	void OnResize(const SRenderDestinationDesc* pDescription);

	long MainWindowProc(ZApplicationEngineWin32* applicationEngineWin32, HWND hWnd, unsigned int uMsgId, unsigned int wParam, long lParam);

	HitmanAbsolutionSDK_API ImGuiContext* GetImGuiContext();
	HitmanAbsolutionSDK_API ImGuiMemAllocFunc GetImGuiMemAllocFunc();
	HitmanAbsolutionSDK_API ImGuiMemFreeFunc GetImGuiMemFreeFunc();
	HitmanAbsolutionSDK_API void* GetImGuiUserDataAllocator();
	HitmanAbsolutionSDK_API ImFont* GetRegularFont();
	HitmanAbsolutionSDK_API ImFont* GetBoldFont();

	std::shared_ptr<ModManager> GetModManager();

	std::shared_ptr<ModSelector> GetModSelector();

private:
	SDK();
	SDK(const SDK& other) = delete;
	SDK& operator=(const SDK& other) = delete;

	std::shared_ptr<DirectXRenderer> directxRenderer;
	std::shared_ptr<ImGuiRenderer> imGuiRenderer;

	std::shared_ptr<ModManager> modManager;

	std::shared_ptr<MainMenu> mainMenu;
	std::shared_ptr<ModSelector> modSelector;
};
