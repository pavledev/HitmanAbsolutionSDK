#pragma once

#include "imgui.h"

#include "IImGuiRenderer.h"
#include "Hooks.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class ZApplicationEngineWin32;
class ZEngineAppCommon;
class ZMouseWindows;
class ZKeyboardWindows;

class ImGuiRenderer final : public IImGuiRenderer
{
  public:
    ImGuiRenderer();
    ~ImGuiRenderer() override;

    void TeardownRenderer();

    void OnEngineInitialized();
    void OnPresent(ZRenderDevice* p_RenderDevice);
    void OnResize(const SRenderDestinationDesc* p_Description);

    bool IsVisible() const override
    {
        return m_IsImGuiVisible.load(std::memory_order_acquire);
    }

    void SetFocus(bool p_HasFocus) override
    {
        m_ImGuiHasFocus.store(p_HasFocus, std::memory_order_release);
    }

    ImGuiContext* GetContext() const override
    {
        return m_ImGuiContext;
    }

    ImPlotContext* GetImPlotContext() const override
    {
        return m_ImPlotContext;
    }

    ImGuiMemAllocFunc GetMemAlloc() const override
    {
        ImGuiMemAllocFunc allocFunc;
        ImGuiMemFreeFunc freeFunc;
        void* userData;
        ImGui::GetAllocatorFunctions(&allocFunc, &freeFunc, &userData);

        return allocFunc;
    }

    ImGuiMemFreeFunc GetMemFree() const override
    {
        ImGuiMemAllocFunc allocFunc;
        ImGuiMemFreeFunc freeFunc;
        void* userData;
        ImGui::GetAllocatorFunctions(&allocFunc, &freeFunc, &userData);

        return freeFunc;
    }

    void* GetAllocatorUserData() const override
    {
        ImGuiMemAllocFunc allocFunc;
        ImGuiMemFreeFunc freeFunc;
        void* userData;
        ImGui::GetAllocatorFunctions(&allocFunc, &freeFunc, &userData);

        return userData;
    }

    ImFont* GetLightFont() const override
    {
        return m_FontLight;
    }

    ImFont* GetRegularFont() const override
    {
        return m_FontRegular;
    }

    ImFont* GetMediumFont() const override
    {
        return m_FontMedium;
    }

    ImFont* GetBoldFont() const override
    {
        return m_FontBold;
    }

    ImFont* GetBlackFont() const override
    {
        return m_FontBlack;
    }

  private:
    bool SetupRenderer();
    void Draw();
    void SetupStyles();

    DECLARE_STDCALL_DETOUR_WITH_CONTEXT(
        ImGuiRenderer, LRESULT, ZApplicationEngineWin32_MainWindowProc, ZApplicationEngineWin32* p_ApplicationEngineWin32, HWND p_HWnd, UINT p_MsgId,
        WPARAM p_WParam, LPARAM p_LParam
    );

    DECLARE_THISCALL_DETOUR_WITH_CONTEXT(ImGuiRenderer, void, ZMouseWindows_Update, ZMouseWindows* p_MouseWindows, bool p_IgnoreOldEvents);
    DECLARE_THISCALL_DETOUR_WITH_CONTEXT(ImGuiRenderer, void, ZKeyboardWindows_Update, ZKeyboardWindows* p_KeyboardWindows, bool p_IgnoreOldEvents);

    bool m_IsRendererSetup = false;

    HWND m_Hwnd = nullptr;

    ImGuiContext* m_ImGuiContext = nullptr;
    ImPlotContext* m_ImPlotContext = nullptr;

    ImFont* m_FontLight = nullptr;
    ImFont* m_FontRegular = nullptr;
    ImFont* m_FontMedium = nullptr;
    ImFont* m_FontBold = nullptr;
    ImFont* m_FontBlack = nullptr;

    std::atomic<bool> m_ImGuiHasFocus{ false };
    std::atomic<bool> m_IsImGuiVisible{ true };
    std::atomic<bool> m_UIToggleWarningRequested{ false };
    bool m_ShowingUIToggleWarning = false;
};
