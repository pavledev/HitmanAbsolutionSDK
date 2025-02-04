#include <IconsMaterialDesign.h>

#include "imgui.h"

#include "Glacier/ZGameLoopManager.h"
#include "Glacier/UI/ZHUDManager.h"
#include "Glacier/ZLevelManager.h"

#include "Global.h"
#include "HUD.h"

HUD::HUD() :
    isOpen(false),
    toggleHUDAction("ToggleHUD")
{
    uiDisableHUD = reinterpret_cast<int*>(BaseAddress + 0xD5644C);
}

void HUD::OnEngineInitialized()
{
    const ZMemberDelegate<HUD, void(const SGameUpdateEvent&)> delegate(this, &HUD::OnFrameUpdate);

    GameLoopManager->RegisterForFrameUpdate(delegate, 1);

    AddBindings();
}

void HUD::OnDrawMenu()
{
    if (ImGui::Button(ICON_MD_MONITOR " HUD"))
    {
        isOpen = !isOpen;
    }
}

void HUD::OnDrawUI(const bool hasFocus)
{
    if (!hasFocus || !isOpen)
    {
        return;
    }

    ImGui::PushFont(SDK::GetInstance().GetBoldFont());
    ImGui::SetNextWindowSize(ImVec2(1250, 850), ImGuiCond_FirstUseEver);

    const bool isWindowVisible = ImGui::Begin(ICON_MD_MONITOR " HUD", &isOpen, ImGuiWindowFlags_NoScrollbar);

    ImGui::PushFont(SDK::GetInstance().GetRegularFont());

    if (isWindowVisible)
    {
        isHUDVisible = *uiDisableHUD == 0;
        isHealthBarVisible = IsHUDItemVisible("_root.g_mcHealthBar");
        isMinimapVisible = IsHUDItemVisible("_root.g_mcMinimap");
        isTrespassingIconVisible = IsHUDItemVisible("_root.g_mcMinimap.icons.trespass");
        isWeaponDisplayVisible = IsHUDItemVisible("_root.g_mcWeaponDisplay");
        isWeaponSelectorVisible = IsHUDItemVisible("_root.g_mcWeaponSelectorMKB");
        isFocusBarVisible = IsHUDItemVisible("_root.g_mcFocusBar");
        isRatingTrackerVisible = IsHUDItemVisible("_root.g_mcRating");
        isTargetTrackerVisible = IsHUDItemVisible("_root.g_mcContractInfo");
        isAttentionVisible = IsHUDItemVisible("_root.g_mcAttention");
        isCrosshairVisible = IsHUDItemVisible("_root.g_mcReticules");

        if (ImGui::Checkbox("HUD", &isHUDVisible))
        {
            *uiDisableHUD = !*uiDisableHUD;
        }

        if (ImGui::Checkbox("Health Bar", &isHealthBarVisible))
        {
            ToggleHUDItem("_root.g_mcHealthBar", isHealthBarVisible);
        }

        if (ImGui::Checkbox("Minimap", &isMinimapVisible))
        {
            ToggleHUDItem("_root.g_mcMinimap", isMinimapVisible);
            ToggleHUDItem("_root.g_mcMinimapBG", isMinimapVisible);
        }

        if (ImGui::Checkbox("Trespassing Icon", &isTrespassingIconVisible))
        {
            ToggleHUDItem("_root.g_mcMinimap.icons.trespass", isTrespassingIconVisible);
        }

        if (ImGui::Checkbox("Weapon Display", &isWeaponDisplayVisible))
        {
            ToggleHUDItem("_root.g_mcWeaponDisplay", isWeaponDisplayVisible);
        }

        if (ImGui::Checkbox("Weapon Selector", &isWeaponSelectorVisible))
        {
            ToggleHUDItem("_root.g_mcWeaponSelectorMKB", isWeaponSelectorVisible);
        }

        if (ImGui::Checkbox("Focus Bar", &isFocusBarVisible))
        {
            ToggleHUDItem("_root.g_mcFocusBar", isFocusBarVisible);
        }

        if (ImGui::Checkbox("Rating Tracker", &isRatingTrackerVisible))
        {
            ToggleHUDItem("_root.g_mcRating", isRatingTrackerVisible);
        }

        if (ImGui::Checkbox("Target Tracker", &isTargetTrackerVisible))
        {
            ToggleHUDItem("_root.g_mcContractInfo", isTargetTrackerVisible);
        }

        if (ImGui::Checkbox("Attention", &isAttentionVisible))
        {
            ToggleHUDItem("_root.g_mcAttention", isAttentionVisible);
        }

        if (ImGui::Checkbox("Crosshair", &isCrosshairVisible))
        {
            ToggleHUDItem("_root.g_mcReticules", isCrosshairVisible);
        }

        ImGui::Separator();

        if (ImGui::Button("Ok"))
        {
            isOpen = false;
        }
    }

    ImGui::PopFont();
    ImGui::End();
    ImGui::PopFont();
}

const bool HUD::IsHUDItemVisible(const char* name)
{
    GFxValue value;
    GFxValue visibleValue;

    HUDManager->GetHUD()->GetMember(name, &value);
    value.GetMember("_visible", &visibleValue);

    return visibleValue.GetBool();
}

void HUD::ToggleHUDItem(const char* name, const bool show)
{
    GFxValue value;
    GFxValue visibleValue;

    HUDManager->GetHUD()->GetMember(name, &value);
    value.GetMember("_visible", &visibleValue);

    visibleValue.SetBool(show);

    value.SetMember("_visible", visibleValue);
}

void HUD::OnFrameUpdate(const SGameUpdateEvent& updateEvent)
{
    ZHitman5* hitman = LevelManager->GetHitman().GetRawPointer();

    if (!hitman)
    {
        return;
    }

    if (toggleHUDAction.Digital())
    {
        *uiDisableHUD = !*uiDisableHUD;
    }
}

DEFINE_MOD(HUD);
