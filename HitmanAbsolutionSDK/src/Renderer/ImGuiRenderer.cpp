#include <MinHook.h>

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include <IconsMaterialDesign.h>

#include "Glacier/Render/ZRenderManager.h"
#include "Glacier/ZGraphicsSettingsManager.h"
#include "Glacier/Input/ZInputActionManager.h"
#include "Glacier/UI/ZGameWideUI.h"
#include "Glacier/UI/ZScaleformManager.h"
#include "Glacier/UI/ZHUDManager.h"

#include "Renderer/ImGuiRenderer.h"
#include "Global.h"
#include "Utility/MinHookUtility.h"
#include "Hooks.h"
#include "Mutex.h"
#include "SDK.h"

#include <imgui_internal.h>
#include <unordered_set>

ImGuiRenderer::ImGuiRenderer()
{
    isRendererSetup = false;
    imguiHasFocus = false;
    regularFont = nullptr;
    boldFont = nullptr;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;
    //io.ConfigViewportsNoDefaultParent = true;
    //io.ConfigDockingAlwaysTabBar = true;
    //io.ConfigDockingTransparentPayload = true;
    //io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;     // FIXME-DPI: Experimental. THIS CURRENTLY DOESN'T WORK AS EXPECTED. DON'T USE IN USER APP!
    //io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports; // FIXME-DPI: Experimental.

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    SetStyle();
}

ImGuiRenderer::~ImGuiRenderer()
{
}

bool ImGuiRenderer::Setup()
{
    if (isRendererSetup)
    {
        return true;
    }

    ZRenderDevice* renderDevice = RenderManager->GetRenderDevice();

    if (!ImGui_ImplWin32_Init(GraphicsSettingsManager->GetHWND()))
    {
        return false;
    }

    if (!ImGui_ImplDX11_Init(renderDevice->GetDirect3DDevice(), renderDevice->GetImmediateContext()))
    {
        return false;
    }

    AddFonts();
    SetScale();

    Logger::GetInstance().Log(Logger::Level::Info, "ImGui renderer successfully set up.");

    isRendererSetup = true;

    return true;
}

void ImGuiRenderer::OnPresent(ZRenderDevice* renderDevice)
{
    if (!Setup())
    {
        Logger::GetInstance().Log(Logger::Level::Error, "Failed to set up ImGui renderer.");
        Cleanup();

        return;
    }

    Render();
}

void ImGuiRenderer::Render()
{
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    RenderContent();

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

void ImGuiRenderer::RenderContent()
{
    SDK::GetInstance().OnDrawUI(imguiHasFocus);
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
    static constexpr const char* regularFontPath = "assets/fonts/Roboto-Regular.ttf";
    static constexpr const char* boldFontPath = "assets/fonts/Roboto-Bold.ttf";
    static constexpr const char* materialIconsRegularFontPath = "assets/fonts/MaterialIcons-Regular.ttf";

    regularFont = io.Fonts->AddFontFromFileTTF(regularFontPath, 32.f);
    io.Fonts->AddFontFromFileTTF(materialIconsRegularFontPath, 32.f, &iconsConfig, iconRanges);
    io.Fonts->Build();

    boldFont = io.Fonts->AddFontFromFileTTF(boldFontPath, 32.f);
    io.Fonts->AddFontFromFileTTF(materialIconsRegularFontPath, 32.f, &iconsConfig, iconRanges);
    io.Fonts->Build();

    io.FontDefault = regularFont;
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

    style.WindowPadding = ImVec2(20.f, 20.f);
    style.FramePadding = ImVec2(10.f, 10.f);
    style.CellPadding = ImVec2(5.f, 5.f);
    style.ItemSpacing = ImVec2(20.f, 10.f);
    style.ItemInnerSpacing = ImVec2(10.f, 10.f);
    style.TouchExtraPadding = ImVec2(0.f, 0.f);
    style.IndentSpacing = 20.f;
    style.ScrollbarSize = 20.f;
    style.GrabMinSize = 20.f;

    /*style.WindowBorderSize = 0.f;
    style.ChildBorderSize = 0.f;
    style.PopupBorderSize = 0.f;
    style.FrameBorderSize = 0.f;
    style.TabBorderSize = 0.f;*/
    style.FrameBorderSize = 1.f;

    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.141f, 0.141f, 0.145f, 1.f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.11f, 0.11f, 0.14f, 0.92f);
    colors[ImGuiCol_Border] = ImVec4(0.50f, 0.50f, 0.50f, 0.50f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.141f, 0.141f, 0.145f, 1.f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.141f, 0.141f, 0.145f, 1.f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.141f, 0.141f, 0.145f, 1.f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.427f, 0.023f, 0.423f, 1.f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.427f, 0.023f, 0.423f, 1.f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.40f, 0.40f, 0.80f, 0.20f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.141f, 0.141f, 0.145f, 1.f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.25f, 0.30f, 0.60f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.40f, 0.40f, 0.80f, 0.30f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.80f, 0.40f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.41f, 0.39f, 0.80f, 0.60f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.427f, 0.023f, 0.423f, 1.f);
    colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.41f, 0.39f, 0.80f, 0.60f);
    colors[ImGuiCol_Button] = ImVec4(0.349f, 0.023f, 0.427f, 1.f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.427f, 0.023f, 0.423f, 1.f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.427f, 0.023f, 0.423f, 1.f);
    colors[ImGuiCol_Header] = ImVec4(0.40f, 0.40f, 0.90f, 0.45f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.45f, 0.45f, 0.90f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.53f, 0.53f, 0.87f, 0.80f);
    colors[ImGuiCol_Separator] = ImVec4(0.50f, 0.50f, 0.50f, 0.60f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.60f, 0.60f, 0.70f, 1.00f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.70f, 0.70f, 0.90f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.78f, 0.82f, 1.00f, 0.60f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.78f, 0.82f, 1.00f, 0.90f);
    colors[ImGuiCol_Tab] = ImLerp(colors[ImGuiCol_Header], colors[ImGuiCol_TitleBgActive], 0.80f);
    colors[ImGuiCol_TabHovered] = colors[ImGuiCol_HeaderHovered];
    colors[ImGuiCol_TabActive] = ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
    colors[ImGuiCol_TabUnfocused] = ImLerp(colors[ImGuiCol_Tab], colors[ImGuiCol_TitleBg], 0.80f);
    colors[ImGuiCol_TabUnfocusedActive] = ImLerp(colors[ImGuiCol_TabActive], colors[ImGuiCol_TitleBg], 0.40f);
    colors[ImGuiCol_DockingPreview] = colors[ImGuiCol_Header] * ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
    colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg] = ImVec4(0.27f, 0.27f, 0.38f, 1.00f);
    colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.45f, 1.00f);   // Prefer using Alpha=1.0 here
    colors[ImGuiCol_TableBorderLight] = ImVec4(0.26f, 0.26f, 0.28f, 1.00f);   // Prefer using Alpha=1.0 here
    colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.07f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight] = colors[ImGuiCol_HeaderHovered];
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}

void ImGuiRenderer::SetScale()
{
    ImGuiIO& io = ImGui::GetIO();
    const HWND hwnd = GraphicsSettingsManager->GetHWND();
    RECT rect = { 0, 0, 0, 0 };

    GetClientRect(hwnd, &rect);

    io.DisplaySize = ImVec2(static_cast<float>(rect.right - rect.left), static_cast<float>(rect.bottom - rect.top));
    io.FontGlobalScale = (io.DisplaySize.y / 2048.f);
}

long ImGuiRenderer::MainWindowProc(ZApplicationEngineWin32* applicationEngineWin32, HWND hWnd, unsigned int uMsgId, unsigned int wParam, long lParam)
{
    if (!ImGui::GetCurrentContext())
    {
        return Hooks::ZApplicationEngineWin32_MainWindowProc.CallOriginalFunction(applicationEngineWin32, hWnd, uMsgId, wParam, lParam);
    }

    unsigned char scanCode = static_cast<unsigned char>(lParam >> 16);

    //Grave/Tilde key = 0x29
    if (scanCode == 0x29 && (uMsgId == WM_KEYDOWN || uMsgId == WM_SYSKEYDOWN))
    {
        imguiHasFocus = !imguiHasFocus;
    }

    InputActionManager->SetEnabled(!imguiHasFocus);

    ZGameWideUIScaleformHandler* gameWideUIScaleformHandler = GameWideUI->GetGameWideUIScaleformHandler();

    if (gameWideUIScaleformHandler && !ScaleformManager->IsInMainMenu() && !HUDManager->IsPauseMenuActive())
    {
        gameWideUIScaleformHandler->ShowUICursor(imguiHasFocus);
    }

    if (uMsgId == WM_QUIT || uMsgId == WM_DESTROY || uMsgId == WM_NCDESTROY || uMsgId == WM_CLOSE)
    {
        imguiHasFocus = false;

        return Hooks::ZApplicationEngineWin32_MainWindowProc.CallOriginalFunction(applicationEngineWin32, hWnd, uMsgId, wParam, lParam);
    }

    if (imguiHasFocus)
    {
        ImGui_ImplWin32_WndProcHandler(hWnd, uMsgId, wParam, lParam);

        //Block mouse and keyboard input in main menu and pause menu
        if (ScaleformManager->IsInMainMenu() || HUDManager->IsPauseMenuActive()) 
        {
            return DefWindowProcW(hWnd, uMsgId, wParam, lParam);
        }
    }

    return Hooks::ZApplicationEngineWin32_MainWindowProc.CallOriginalFunction(applicationEngineWin32, hWnd, uMsgId, wParam, lParam);
}

void ImGuiRenderer::OnUpdateInputDeviceManager(ZEngineAppCommon* engineAppCommon)
{
    //Block mouse and keyboard input in main menu and pause menu
    if (imguiHasFocus && (ScaleformManager->IsInMainMenu() || HUDManager->IsPauseMenuActive()))
    {
        return;
    }

    return Hooks::ZEngineAppCommon_UpdateInputDeviceManager.CallOriginalFunction(engineAppCommon);
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
    return regularFont;
}

ImFont* ImGuiRenderer::GetBoldFont()
{
    return boldFont;
}
