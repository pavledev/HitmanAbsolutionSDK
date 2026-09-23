#include <MinHook.h>

#include <IconsMaterialDesign.h>

#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#include "Glacier/ZRender.h"
#include "Glacier/ZGraphicsSettingsManager.h"
#include "Glacier/ZInput.h"
#include "Glacier/ZScaleform.h"
#include "Glacier/ZApplication.h"

#include "ImGuiRenderer.h"
#include "Globals.h"
#include "Hooks.h"
#include "ModSDK.h"
#include "Fonts.h"

ImGuiRenderer::ImGuiRenderer()
{
    IMGUI_CHECKVERSION();
    m_ImGuiContext = ImGui::CreateContext();
    m_ImPlotContext = ImPlot::CreateContext();

    ImGuiIO& imGuiIO = ImGui::GetIO();
    imGuiIO.IniFilename = nullptr;
    imGuiIO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    imGuiIO.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    imGuiIO.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
    imGuiIO.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

    ImFontConfig iconsConfig{};
    iconsConfig.MergeMode = true;
    iconsConfig.GlyphOffset = { 0.f, 6.f };

    // Unicode ranges used by ImGui font
    static constexpr ImWchar c_TextRanges[] = { 0x0020, 0x00FF, // Basic Latin + Latin-1 Supplement
                                                0x2010, 0x2027, // Punctuation
                                                0 };
    static constexpr ImWchar c_IconRanges[] = { ICON_MIN_MD, ICON_MAX_16_MD, 0 };

    m_FontLight = imGuiIO.Fonts->AddFontFromMemoryCompressedTTF(RobotoLightCompressedData, RobotoLightCompressedSize, 28.f, nullptr, c_TextRanges);
    imGuiIO.Fonts->AddFontFromMemoryCompressedTTF(
        MaterialIconsRegularCompressedData, MaterialIconsRegularCompressedSize, 28.f, &iconsConfig, c_IconRanges
    );

    m_FontRegular =
        imGuiIO.Fonts->AddFontFromMemoryCompressedTTF(RobotoRegularCompressedData, RobotoRegularCompressedSize, 28.f, nullptr, c_TextRanges);
    imGuiIO.Fonts->AddFontFromMemoryCompressedTTF(
        MaterialIconsRegularCompressedData, MaterialIconsRegularCompressedSize, 28.f, &iconsConfig, c_IconRanges
    );

    m_FontMedium = imGuiIO.Fonts->AddFontFromMemoryCompressedTTF(RobotoMediumCompressedData, RobotoMediumCompressedSize, 28.f, nullptr, c_TextRanges);
    imGuiIO.Fonts->AddFontFromMemoryCompressedTTF(
        MaterialIconsRegularCompressedData, MaterialIconsRegularCompressedSize, 28.f, &iconsConfig, c_IconRanges
    );

    m_FontBold = imGuiIO.Fonts->AddFontFromMemoryCompressedTTF(RobotoBoldCompressedData, RobotoBoldCompressedSize, 28.f, nullptr, c_TextRanges);
    imGuiIO.Fonts->AddFontFromMemoryCompressedTTF(
        MaterialIconsRegularCompressedData, MaterialIconsRegularCompressedSize, 28.f, &iconsConfig, c_IconRanges
    );

    m_FontBlack = imGuiIO.Fonts->AddFontFromMemoryCompressedTTF(RobotoBlackCompressedData, RobotoBlackCompressedSize, 28.f, nullptr, c_TextRanges);
    imGuiIO.Fonts->AddFontFromMemoryCompressedTTF(
        MaterialIconsRegularCompressedData, MaterialIconsRegularCompressedSize, 28.f, &iconsConfig, c_IconRanges
    );

    imGuiIO.FontDefault = m_FontRegular;

    SetupStyles();
}

ImGuiRenderer::~ImGuiRenderer()
{
    TeardownRenderer();

    ImGui::DestroyContext(m_ImGuiContext);
    ImPlot::DestroyContext(m_ImPlotContext);

    m_ImGuiContext = nullptr;
    m_ImPlotContext = nullptr;
}

void ImGuiRenderer::OnEngineInitialized()
{
    Hooks::ZApplicationEngineWin32_MainWindowProc->AddDetour(this, &ImGuiRenderer::ZApplicationEngineWin32_MainWindowProc);

    Hooks::ZMouseWindows_Update->AddDetour(this, &ImGuiRenderer::ZMouseWindows_Update);
    Hooks::ZKeyboardWindows_Update->AddDetour(this, &ImGuiRenderer::ZKeyboardWindows_Update);
}

void ImGuiRenderer::OnPresent(ZRenderDevice* p_RenderDevice)
{
    if (!SetupRenderer())
    {
        Logger::Error("[ImGuiRenderer] Failed to set up renderer.");
        return;
    }

    if (!m_IsImGuiVisible.load(std::memory_order_acquire))
    {
        return;
    }

    Draw();
}

void ImGuiRenderer::OnResize(const SRenderDestinationDesc* p_Description)
{
    ImGuiIO& imGuiIO = ImGui::GetIO();

    RECT rect = { 0, 0, 0, 0 };
    GetClientRect(m_Hwnd, &rect);

    imGuiIO.DisplaySize = ImVec2(static_cast<float>(rect.right - rect.left), static_cast<float>(rect.bottom - rect.top));
    imGuiIO.FontGlobalScale = imGuiIO.DisplaySize.y / 1800.f;
}

bool ImGuiRenderer::SetupRenderer()
{
    if (m_IsRendererSetup)
    {
        return true;
    }

    m_Hwnd = Globals::GraphicsSettingsManager->m_hWnd;

    if (!ImGui_ImplWin32_Init(m_Hwnd))
    {
        return false;
    }

    if (!ImGui_ImplDX11_Init(
            Globals::RenderManager->m_pRenderDevice->m_pDirect3DDevice, Globals::RenderManager->m_pRenderDevice->m_pDeviceContextImmediate
        ))
    {
        return false;
    }

    ImGuiIO& imGuiIO = ImGui::GetIO();
    RECT rect = { 0, 0, 0, 0 };

    GetClientRect(m_Hwnd, &rect);

    imGuiIO.DisplaySize = ImVec2(static_cast<float>(rect.right - rect.left), static_cast<float>(rect.bottom - rect.top));
    imGuiIO.FontGlobalScale = imGuiIO.DisplaySize.y / 1800.f;

    ImGui::GetMainViewport()->PlatformHandleRaw = m_Hwnd;

    m_IsRendererSetup = true;

    Logger::Info("[ImGuiRenderer] Renderer ready (hwnd={}).", static_cast<void*>(m_Hwnd));

    return true;
}

void ImGuiRenderer::TeardownRenderer()
{
    if (!m_IsRendererSetup)
    {
        return;
    }

    m_IsRendererSetup = false;

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
}

void ImGuiRenderer::Draw()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    const bool hasFocus = m_ImGuiHasFocus.load(std::memory_order_acquire);

    ImGui::GetStyle().Alpha = hasFocus ? 1.f : 0.3f;

    ModSDK::GetInstance().OnDrawUI(hasFocus);

    if (m_UIToggleWarningRequested.exchange(false, std::memory_order_acquire))
    {
        m_ShowingUIToggleWarning = true;
    }

    if (m_ShowingUIToggleWarning)
    {
        const auto center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        ImGui::PushFont(m_FontBlack);
        const auto isWindowExpanded = ImGui::Begin("Warning", &m_ShowingUIToggleWarning);
        ImGui::PushFont(m_FontRegular);

        if (isWindowExpanded)
        {
            ImGui::Text("You have pressed the UI toggle key (F11 by default), which will HIDE the SDK UI.");
            ImGui::Text("You must press this key again to show the SDK UI.");
            ImGui::Text("If you want to change this key, you can do so in the mods.ini file.");
            ImGui::Text("See the SDK readme for more information. This warning will not appear again.");

            ImGui::NewLine();

            static bool hasConfirmed = false;

            ImGui::Checkbox("I understand I'm hiding the UI and that I must press this key to show it again", &hasConfirmed);

            ImGui::NewLine();

            ImGui::BeginDisabled(!hasConfirmed);

            if (ImGui::Button("Continue"))
            {
                ModSDK::GetInstance().SetHasShownUIToggleWarning();
                m_IsImGuiVisible.store(false, std::memory_order_release);
                m_ShowingUIToggleWarning = false;
                m_ImGuiHasFocus.store(false, std::memory_order_release);
            }

            ImGui::EndDisabled();

            ImGui::SameLine();

            if (ImGui::Button("Cancel"))
            {
                m_ShowingUIToggleWarning = false;
            }
        }

        ImGui::PopFont();
        ImGui::End();
        ImGui::PopFont();
    }

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiRenderer::SetupStyles()
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
    colors[ImGuiCol_CheckboxSelectedBg] = ImVec4(0.06f, 0.05f, 0.05f, 1.00f);
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

DEFINE_STDCALL_DETOUR_WITH_CONTEXT(
    ImGuiRenderer, LRESULT, ZApplicationEngineWin32_MainWindowProc, ZApplicationEngineWin32* p_ApplicationEngineWin32, HWND p_HWnd, UINT p_MsgId,
    WPARAM p_WParam, LPARAM p_LParam
)
{
    if (!ImGui::GetCurrentContext())
    {
        return { HookAction::Continue() };
    }

    // Layout-independent tilde toggle: compare the hardware scancode
    // (lParam bits 16-23) rather than VK_OEM_3, which moves per layout.
    const uint8_t scanCode = static_cast<uint8_t>(p_LParam >> 16);

    if (scanCode == ModSDK::GetInstance().GetConsoleScanCode() && (p_MsgId == WM_KEYDOWN || p_MsgId == WM_SYSKEYDOWN))
    {
        const bool newFocus = !m_ImGuiHasFocus.load(std::memory_order_relaxed);
        m_ImGuiHasFocus.store(newFocus, std::memory_order_release);

        if (newFocus)
        {
            m_IsImGuiVisible.store(true, std::memory_order_release);
        }

        if (!Globals::ScaleformManager->m_bIsInMainMenu && !Globals::HUDManager->m_bPauseMenuActive)
        {
            if (newFocus)
            {
                SetCursor(p_ApplicationEngineWin32->m_hDefaultCursor);
            }
            else
            {
                SetCursor(nullptr);
            }

            ShowCursor(newFocus);

            p_ApplicationEngineWin32->m_bShowingCursor = newFocus;
        }
    }

    if (scanCode == ModSDK::GetInstance().GetUIToggleScanCode() && (p_MsgId == WM_KEYDOWN || p_MsgId == WM_SYSKEYDOWN))
    {
        if (!ModSDK::GetInstance().HasShownUIToggleWarning())
        {
            m_UIToggleWarningRequested.store(true, std::memory_order_release);
            m_ImGuiHasFocus.store(true, std::memory_order_release);
        }
        else
        {
            const bool newVisible = !m_IsImGuiVisible.load(std::memory_order_relaxed);
            m_IsImGuiVisible.store(newVisible, std::memory_order_release);

            if (!newVisible)
            {
                m_ImGuiHasFocus.store(false, std::memory_order_release);
            }
        }
    }

    const bool hasFocus = m_ImGuiHasFocus.load(std::memory_order_acquire);

    Globals::InputActionManager->m_bEnabled = !hasFocus;

    if (!hasFocus)
    {
        return { HookAction::Continue() };
    }

    if (p_MsgId == WM_QUIT || p_MsgId == WM_DESTROY || p_MsgId == WM_NCDESTROY || p_MsgId == WM_CLOSE)
    {
        m_ImGuiHasFocus.store(false, std::memory_order_release);

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
    if (m_ImGuiHasFocus.load(std::memory_order_acquire) && (Globals::ScaleformManager->m_bIsInMainMenu || Globals::HUDManager->m_bPauseMenuActive))
    {
        return { HookAction::Return() };
    }

    return { HookAction::Continue() };
}

DEFINE_THISCALL_DETOUR_WITH_CONTEXT(ImGuiRenderer, void, ZKeyboardWindows_Update, ZKeyboardWindows* p_KeyboardWindows, bool p_IgnoreOldEvents)
{
    // Block mouse and keyboard input in main menu and pause menu
    if (m_ImGuiHasFocus.load(std::memory_order_acquire) && (Globals::ScaleformManager->m_bIsInMainMenu || Globals::HUDManager->m_bPauseMenuActive))
    {
        return { HookAction::Return() };
    }

    return { HookAction::Continue() };
}
