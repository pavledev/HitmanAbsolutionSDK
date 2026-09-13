#pragma once

#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"

#include "DirectXRenderer.h"
#include "Hooks.h"

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

    void OnEngineInitialized();

    bool Setup();
    void Render();
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

    void OnPresent(ZRenderDevice* p_RenderDevice);
    void OnResize(const SRenderDestinationDesc* p_Description);

  private:
    DECLARE_STDCALL_DETOUR_WITH_CONTEXT(
        ImGuiRenderer, LRESULT, ZApplicationEngineWin32_MainWindowProc, ZApplicationEngineWin32* p_ApplicationEngineWin32, HWND p_HWnd, UINT p_MsgId,
        WPARAM p_WParam, LPARAM p_LParam
    );

    DECLARE_THISCALL_DETOUR_WITH_CONTEXT(ImGuiRenderer, void, ZMouseWindows_Update, ZMouseWindows* p_MouseWindows, bool p_IgnoreOldEvents);
    DECLARE_THISCALL_DETOUR_WITH_CONTEXT(ImGuiRenderer, void, ZKeyboardWindows_Update, ZKeyboardWindows* p_KeyboardWindows, bool p_IgnoreOldEvents);

    bool m_IsRendererSetup;
    volatile bool m_ImguiHasFocus;

    ImFont* m_RegularFont;
    ImFont* m_BoldFont;
};
