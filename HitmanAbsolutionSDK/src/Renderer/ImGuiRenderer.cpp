#include <MinHook.h>

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include <imgui_internal.h>

#include <IconsMaterialDesign.h>

#include "Glacier/ZRender.h"
#include "Glacier/ZGraphicsSettingsManager.h"
#include "Glacier/ZInput.h"
#include "Glacier/ZScaleform.h"
#include "Glacier/ZApplication.h"

#include "Renderer/ImGuiRenderer.h"
#include "Globals.h"
#include "Hooks.h"
#include "SDK.h"
#include "Fonts.h"

ImGuiRenderer::ImGuiRenderer()
{
    m_IsRendererSetup = false;
    m_ImguiHasFocus = false;
    m_RegularFont = nullptr;
    m_BoldFont = nullptr;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    // io.ConfigViewportsNoAutoMerge = true;
    // io.ConfigViewportsNoTaskBarIcon = true;
    // io.ConfigViewportsNoDefaultParent = true;
    // io.ConfigDockingAlwaysTabBar = true;
    // io.ConfigDockingTransparentPayload = true;
    // io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;     // FIXME-DPI: Experimental. THIS CURRENTLY DOESN'T WORK AS EXPECTED. DON'T USE IN
    // USER APP! io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports; // FIXME-DPI: Experimental.

    // ImGui::StyleColorsDark();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    SetStyle();
}

ImGuiRenderer::~ImGuiRenderer() {}

void ImGuiRenderer::OnEngineInitialized()
{
    Hooks::ZApplicationEngineWin32_MainWindowProc->AddDetour(this, &ImGuiRenderer::ZApplicationEngineWin32_MainWindowProc);

    Hooks::ZMouseWindows_Update->AddDetour(this, &ImGuiRenderer::ZMouseWindows_Update);
    Hooks::ZKeyboardWindows_Update->AddDetour(this, &ImGuiRenderer::ZKeyboardWindows_Update);
}

bool ImGuiRenderer::Setup()
{
    if (m_IsRendererSetup)
    {
        return true;
    }

    ZRenderDevice* renderDevice = Globals::RenderManager->m_pRenderDevice;

    if (!ImGui_ImplWin32_Init(Globals::GraphicsSettingsManager->m_hWnd))
    {
        return false;
    }

    if (!ImGui_ImplDX11_Init(renderDevice->m_pDirect3DDevice, renderDevice->m_pDeviceContextImmediate))
    {
        return false;
    }

    AddFonts();
    SetScale();

    Logger::Info("ImGui renderer successfully set up.");

    m_IsRendererSetup = true;

    return true;
}

void ImGuiRenderer::Render()
{
    ImGuiIO& io = ImGui::GetIO();

    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    SDK::GetInstance().OnDrawUI(m_ImguiHasFocus);

    // Rendering
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}

void ImGuiRenderer::Cleanup()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiRenderer::AddFonts()
{
    ImGuiIO& io = ImGui::GetIO();
    ImFontConfig iconsConfig{};
    iconsConfig.MergeMode = true;
    iconsConfig.GlyphOffset = { 0.f, 6.f };

    static constexpr ImWchar iconRanges[] = { ICON_MIN_MD, ICON_MAX_16_MD, 0 };

    m_RegularFont = io.Fonts->AddFontFromMemoryCompressedTTF(RobotoRegularCompressedData, RobotoRegularCompressedSize, 28.f);
    io.Fonts->AddFontFromMemoryCompressedTTF(MaterialIconsRegularCompressedData, MaterialIconsRegularCompressedSize, 28.f, &iconsConfig, iconRanges);
    io.Fonts->Build();

    m_BoldFont = io.Fonts->AddFontFromMemoryCompressedTTF(RobotoBoldCompressedData, RobotoBoldCompressedSize, 28.f);
    io.Fonts->AddFontFromMemoryCompressedTTF(MaterialIconsRegularCompressedData, MaterialIconsRegularCompressedSize, 28.f, &iconsConfig, iconRanges);
    io.Fonts->Build();

    io.FontDefault = m_RegularFont;
}

void ImGuiRenderer::SetStyle()
{
    auto& style = ImGui::GetStyle();

    style.ChildRounding = 0.f;
    style.FrameRounding = 0.f;
    style.GrabRounding = 0.f;
    style.PopupRounding = 0.f;
    style.ScrollbarRounding = 0.f;
    style.TabRounding = 0.f;
    style.WindowRounding = 0.f;
    style.WindowBorderSize = 0.f;

    style.WindowPadding = ImVec2(12.f, 12.f);
    style.FramePadding = ImVec2(6.f, 6.f);
    style.CellPadding = ImVec2(6.f, 3.f);
    style.ItemSpacing = ImVec2(10.f, 6.f);
    style.ItemInnerSpacing = ImVec2(10.f, 10.f);
    style.TouchExtraPadding = ImVec2(0.f, 0.f);
    style.IndentSpacing = 34.f;
    style.ScrollbarSize = 12.f;
    style.GrabMinSize = 12.f;

    style.WindowBorderSize = 0.f;
    style.ChildBorderSize = 0.f;
    style.PopupBorderSize = 0.f;
    style.FrameBorderSize = 0.f;
    style.TabBorderSize = 0.f;

    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.18f, 0.19f, 0.22f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.06f, 0.05f, 0.05f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.13f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.98f, 0.00f, 0.05f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.18f, 0.19f, 0.22f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.98f, 0.00f, 0.05f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.18f, 0.19f, 0.22f, 1.00f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.98f, 0.00f, 0.05f, 1.00f);
    // colors[ImGuiCol_CheckboxSelectedBg] = ImVec4(0.06f, 0.05f, 0.05f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.98f, 0.00f, 0.05f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.05f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.55f, 0.11f, 0.13f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.98f, 0.00f, 0.05f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.05f, 0.05f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.55f, 0.11f, 0.13f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.98f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.98f, 0.00f, 0.05f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.09f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.55f, 0.11f, 0.13f, 1.00f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.98f, 0.00f, 0.05f, 1.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.55f, 0.11f, 0.13f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.98f, 0.00f, 0.05f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.98f, 0.00f, 0.05f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
    colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
    colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
    colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

void ImGuiRenderer::SetScale()
{
    ImGuiIO& io = ImGui::GetIO();
    const HWND hwnd = Globals::GraphicsSettingsManager->m_hWnd;
    RECT rect = { 0, 0, 0, 0 };

    GetClientRect(hwnd, &rect);

    io.DisplaySize = ImVec2(static_cast<float>(rect.right - rect.left), static_cast<float>(rect.bottom - rect.top));
    io.FontGlobalScale = io.DisplaySize.y / 1800.f;
}

ImGuiContext* ImGuiRenderer::GetImGuiContext()
{
    return ImGui::GetCurrentContext();
}

ImGuiMemAllocFunc ImGuiRenderer::GetImGuiMemAllocFunc()
{
    ImGuiMemAllocFunc imGuiMemAllocFunc;
    ImGuiMemFreeFunc imGuiMemFreeFunc;
    void* userData;

    ImGui::GetAllocatorFunctions(&imGuiMemAllocFunc, &imGuiMemFreeFunc, &userData);

    return imGuiMemAllocFunc;
}

ImGuiMemFreeFunc ImGuiRenderer::GetImGuiMemFreeFunc()
{
    ImGuiMemAllocFunc imGuiMemAllocFunc;
    ImGuiMemFreeFunc imGuiMemFreeFunc;
    void* userData;

    ImGui::GetAllocatorFunctions(&imGuiMemAllocFunc, &imGuiMemFreeFunc, &userData);

    return imGuiMemFreeFunc;
}

void* ImGuiRenderer::GetImGuiUserDataAllocator()
{
    ImGuiMemAllocFunc imGuiMemAllocFunc;
    ImGuiMemFreeFunc imGuiMemFreeFunc;
    void* userData;

    ImGui::GetAllocatorFunctions(&imGuiMemAllocFunc, &imGuiMemFreeFunc, &userData);

    return userData;
}

ImFont* ImGuiRenderer::GetRegularFont()
{
    return m_RegularFont;
}

ImFont* ImGuiRenderer::GetBoldFont()
{
    return m_BoldFont;
}

void ImGuiRenderer::OnPresent(ZRenderDevice* p_RenderDevice)
{
    if (!Setup())
    {
        Logger::Error("Failed to set up ImGui renderer.");

        Cleanup();

        return;
    }

    Render();
}

void ImGuiRenderer::OnResize(const SRenderDestinationDesc* p_Description)
{
    SetScale();
}

DEFINE_STDCALL_DETOUR_WITH_CONTEXT(
    ImGuiRenderer, LRESULT, ZApplicationEngineWin32_MainWindowProc, ZApplicationEngineWin32* p_ApplicationEngineWin32, HWND p_HWnd, UINT p_MsgId,
    WPARAM p_WParam, LPARAM p_LParam
)
{
    if (!ImGui::GetCurrentContext())
    {
        return { HookAction::Continue() };
    }

    unsigned char scanCode = static_cast<unsigned char>(p_LParam >> 16);

    // Grave/Tilde key = 0x29
    if (scanCode == 0x29 && (p_MsgId == WM_KEYDOWN || p_MsgId == WM_SYSKEYDOWN))
    {
        m_ImguiHasFocus = !m_ImguiHasFocus;

        if (!Globals::ScaleformManager->m_bIsInMainMenu && !Globals::HUDManager->m_bPauseMenuActive)
        {
            if (m_ImguiHasFocus)
            {
                SetCursor(p_ApplicationEngineWin32->m_hDefaultCursor);
            }
            else
            {
                SetCursor(nullptr);
            }

            ShowCursor(m_ImguiHasFocus);

            p_ApplicationEngineWin32->m_bShowingCursor = m_ImguiHasFocus;
        }
    }

    Globals::InputActionManager->m_bEnabled = !m_ImguiHasFocus;

    if (!m_ImguiHasFocus)
    {
        return { HookAction::Continue() };
    }

    if (p_MsgId == WM_QUIT || p_MsgId == WM_DESTROY || p_MsgId == WM_NCDESTROY || p_MsgId == WM_CLOSE)
    {
        m_ImguiHasFocus = false;

        return { HookAction::Continue() };
    }

    if (p_MsgId == WM_SIZE)
    {
        return { HookAction::Continue() };
    }

    ImGui_ImplWin32_WndProcHandler(p_HWnd, p_MsgId, p_WParam, p_LParam);

    return { HookAction::Return(), DefWindowProcW(p_HWnd, p_MsgId, p_WParam, p_LParam) };
}

DEFINE_THISCALL_DETOUR_WITH_CONTEXT(ImGuiRenderer, void, ZMouseWindows_Update, ZMouseWindows* p_MouseWindows, bool p_IgnoreOldEvents)
{
    // Block mouse and keyboard input in main menu and pause menu
    if (m_ImguiHasFocus && (Globals::ScaleformManager->m_bIsInMainMenu || Globals::HUDManager->m_bPauseMenuActive))
    {
        return { HookAction::Return() };
    }

    return { HookAction::Continue() };
}

DEFINE_THISCALL_DETOUR_WITH_CONTEXT(ImGuiRenderer, void, ZKeyboardWindows_Update, ZKeyboardWindows* p_KeyboardWindows, bool p_IgnoreOldEvents)
{
    // Block mouse and keyboard input in main menu and pause menu
    if (m_ImguiHasFocus && (Globals::ScaleformManager->m_bIsInMainMenu || Globals::HUDManager->m_bPauseMenuActive))
    {
        return { HookAction::Return() };
    }

    return { HookAction::Continue() };
}
