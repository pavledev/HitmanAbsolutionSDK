#include <IconsMaterialDesign.h>

#include "imgui.h"

#include "Glacier/ZGameLoopManager.h"
#include "Glacier/ZLevelManager.h"

#include "HUD.h"

HUD::~HUD()
{
    const ZMemberDelegate<HUD, void(const SGameUpdateEvent&)> delegate(this, &HUD::OnFrameUpdate);
    Globals::GameLoopManager->UnregisterForFrameUpdate(delegate);
}

void HUD::OnEngineInitialized()
{
    const ZMemberDelegate<HUD, void(const SGameUpdateEvent&)> delegate(this, &HUD::OnFrameUpdate);
    Globals::GameLoopManager->RegisterForFrameUpdate(delegate, 1);

    AddBindings();
}

void HUD::OnDrawMenu()
{
    if (ImGui::Button(ICON_MD_MONITOR " HUD"))
    {
        m_ShowWindow = !m_ShowWindow;
    }
}

void HUD::OnDrawUI(const bool hasFocus)
{
    if (!hasFocus || !m_ShowWindow)
    {
        return;
    }

    ImGui::PushFont(SDK::GetInstance().GetBoldFont());
    ImGui::SetNextWindowSize(ImVec2(1250, 850), ImGuiCond_FirstUseEver);

    const bool isWindowExpanded = ImGui::Begin(ICON_MD_MONITOR " HUD", &m_ShowWindow, ImGuiWindowFlags_NoScrollbar);

    ImGui::PushFont(SDK::GetInstance().GetRegularFont());

    if (isWindowExpanded)
    {
        m_IsHUDVisible = Globals::HUDManager->m_bDisplayHUD;
        m_IsHealthBarVisible = IsHUDItemVisible("_root.g_mcHealthBar");
        m_IsMinimapVisible = IsHUDItemVisible("_root.g_mcMinimap");
        m_IsTrespassingIconVisible = IsHUDItemVisible("_root.g_mcMinimap.icons.trespass");
        m_IsWeaponDisplayVisible = IsHUDItemVisible("_root.g_mcWeaponDisplay");
        m_IsWeaponSelectorVisible = IsHUDItemVisible("_root.g_mcWeaponSelectorMKB");
        m_IsFocusBarVisible = IsHUDItemVisible("_root.g_mcFocusBar");
        m_IsRatingTrackerVisible = IsHUDItemVisible("_root.g_mcRating");
        m_IsTargetTrackerVisible = IsHUDItemVisible("_root.g_mcContractInfo");
        m_IsAttentionVisible = IsHUDItemVisible("_root.g_mcAttention");
        m_IsCrosshairVisible = IsHUDItemVisible("_root.g_mcReticules");

        ImGui::BeginDisabled(!Globals::LevelManager->m_rHitman.m_pInterfaceRef);

        if (ImGui::Checkbox("HUD", &m_IsHUDVisible))
        {
            *Globals::UIDisableHUD = !*Globals::UIDisableHUD;
        }

        if (ImGui::Checkbox("Health bar", &m_IsHealthBarVisible))
        {
            ToggleHUDItem("_root.g_mcHealthBar", m_IsHealthBarVisible);
        }

        if (ImGui::Checkbox("Minimap", &m_IsMinimapVisible))
        {
            ToggleHUDItem("_root.g_mcMinimap", m_IsMinimapVisible);
            ToggleHUDItem("_root.g_mcMinimapBG", m_IsMinimapVisible);
        }

        if (ImGui::Checkbox("Trespassing icon", &m_IsTrespassingIconVisible))
        {
            ToggleHUDItem("_root.g_mcMinimap.icons.trespass", m_IsTrespassingIconVisible);
        }

        if (ImGui::Checkbox("Weapon display", &m_IsWeaponDisplayVisible))
        {
            ToggleHUDItem("_root.g_mcWeaponDisplay", m_IsWeaponDisplayVisible);
        }

        if (ImGui::Checkbox("Weapon selector", &m_IsWeaponSelectorVisible))
        {
            ToggleHUDItem("_root.g_mcWeaponSelectorMKB", m_IsWeaponSelectorVisible);
        }

        if (ImGui::Checkbox("Focus bar", &m_IsFocusBarVisible))
        {
            ToggleHUDItem("_root.g_mcFocusBar", m_IsFocusBarVisible);
        }

        if (ImGui::Checkbox("Rating tracker", &m_IsRatingTrackerVisible))
        {
            ToggleHUDItem("_root.g_mcRating", m_IsRatingTrackerVisible);
        }

        if (ImGui::Checkbox("Target tracker", &m_IsTargetTrackerVisible))
        {
            ToggleHUDItem("_root.g_mcContractInfo", m_IsTargetTrackerVisible);
        }

        if (ImGui::Checkbox("Attention", &m_IsAttentionVisible))
        {
            ToggleHUDItem("_root.g_mcAttention", m_IsAttentionVisible);
        }

        if (ImGui::Checkbox("Crosshair", &m_IsCrosshairVisible))
        {
            ToggleHUDItem("_root.g_mcReticules", m_IsCrosshairVisible);
        }

        ImGui::EndDisabled();
    }

    ImGui::PopFont();
    ImGui::End();
    ImGui::PopFont();
}

const bool HUD::IsHUDItemVisible(const char* p_Name)
{
    if (!Globals::HUDManager || !Globals::HUDManager->m_mcHUD)
    {
        return false;
    }

    GFxValue value;
    GFxValue visibleValue;

    Globals::HUDManager->m_mcHUD->GetMember(p_Name, &value);
    value.GetMember("_visible", &visibleValue);

    return visibleValue.GetBool();
}

void HUD::ToggleHUDItem(const char* p_Name, bool p_Show)
{
    if (!Globals::HUDManager || !Globals::HUDManager->m_mcHUD)
    {
        return;
    }

    GFxValue value;
    GFxValue visibleValue;

    Globals::HUDManager->m_mcHUD->GetMember(p_Name, &value);
    value.GetMember("_visible", &visibleValue);

    visibleValue.SetBoolean(p_Show);

    value.SetMember("_visible", visibleValue);
}

void HUD::OnFrameUpdate(const SGameUpdateEvent& p_UpdateEvent)
{
    if (!Globals::LevelManager->m_rHitman.m_pInterfaceRef)
    {
        return;
    }

    if (m_ToggleHUDAction.Digital())
    {
        *Globals::UIDisableHUD = !*Globals::UIDisableHUD;
    }
}

DEFINE_MOD(HUD);
