#pragma once

#include <Glacier/ZScaleform.h>
#include <Glacier/SGameUpdateEvent.h>
#include <Glacier/ZInput.h>

#include <IModInterface.h>

class HUD : public IModInterface
{
  public:
    ~HUD();

    void OnEngineInitialized() override;
    void OnDrawMenu(IImGuiRenderer* p_Renderer) override;
    void OnDrawUI(IImGuiRenderer* p_Renderer, bool p_HasFocus) override;

    const bool IsHUDItemVisible(const char* p_Name);
    void ToggleHUDItem(const char* p_Name, bool p_Show);

  private:
    void OnFrameUpdate(const SGameUpdateEvent& p_UpdateEvent);

    bool m_ShowWindow = false;

    bool m_IsHUDVisible;
    bool m_IsHealthBarVisible;
    bool m_IsMinimapVisible;
    bool m_IsTrespassingIconVisible;
    bool m_IsWeaponDisplayVisible;
    bool m_IsWeaponSelectorVisible;
    bool m_IsFocusBarVisible;
    bool m_IsRatingTrackerVisible;
    bool m_IsTargetTrackerVisible;
    bool m_IsAttentionVisible;
    bool m_IsCrosshairVisible;

    ZInputAction m_ToggleHUDAction{ "ToggleHUD" };
};

DECLARE_HMASDK_MOD(HUD)
