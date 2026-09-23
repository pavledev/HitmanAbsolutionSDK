#pragma once

#include <Glacier/ZToken.h>

#include <IModInterface.h>

class ZHitman5;

class StartingLoadout : public IModInterface
{
  public:
    void OnEngineInitialized() override;
    void OnDrawMenu(IImGuiRenderer* p_Renderer) override;
    void OnDrawUI(IImGuiRenderer* p_Renderer, bool p_HasFocus) override;

  private:
    void RenderOutfitsTabItem();
    void RenderWeaponsTabItem();

    void LoadOutfits();
    void LoadFirearms();

    DECLARE_THISCALL_MOD_DETOUR(StartingLoadout, void, ZHitman5_Activate, ZHitman5* p_Hitman5, const ZString& p_Subset);

    bool m_ShowWindow = false;

    std::map<std::string, STokenID> m_OutfitNameToTokenID;
    std::map<std::string, STokenID> m_FirearmNameToTokenID;

    STokenID m_SelectedOutfit;
    STokenID m_SelectedFirearm;
};

DECLARE_HMASDK_MOD(StartingLoadout)
