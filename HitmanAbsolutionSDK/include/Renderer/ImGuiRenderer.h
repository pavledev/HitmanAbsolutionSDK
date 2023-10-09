#pragma once

#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"

#include "DirectXRenderer.h"

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class ZApplicationEngineWin32;
class ZEngineAppCommon;
class ZMouseWindows;
class ZKeyboardWindows;

class ImGuiRenderer
{
public:
	ImGuiRenderer();
	~ImGuiRenderer();
	bool Setup();
	void Render();
	void RenderContent();
	void Cleanup();

	void AddFonts();
	void SetStyle();
	void SetScale();

	ImGuiContext* GetImGuiContext();
	ImGuiMemAllocFunc GetImGuiMemAllocFunc();
	ImGuiMemFreeFunc GetImGuiMemFreeFunc();
	void* GetImGuiUserDataAllocator();
	ImFont* GetRegularFont();
	ImFont* GetBoldFont();

	void OnPresent(ZRenderDevice* renderDevice);
	void OnResize(const SRenderDestinationDesc* pDescription);

	long OnMainWindowProc(ZApplicationEngineWin32* applicationEngineWin32, HWND hWnd, unsigned int uMsgId, unsigned int wParam, long lParam);
	void OnMouseWindowsUpdate(ZMouseWindows* mouseWindows, bool bIgnoreOldEvents);
	void OnKeyboardWindowsUpdate(ZKeyboardWindows* keyboardWindows, bool bIgnoreOldEvents);

private:
	bool isRendererSetup;
	volatile bool imguiHasFocus;

	ImFont* regularFont;
	ImFont* boldFont;
};
