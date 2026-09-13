#include "StartingLoadout.h"

#include <IconsMaterialDesign.h>

#include <Glacier/ZContentKit.h>
#include <Glacier/ZLevelManager.h>

#include "Hooks.h"
#include "Utils/ImGuiUtils.h"

void StartingLoadout::OnEngineInitialized()
{
    Hooks::ZHitman5_Activate->AddDetour(this, &StartingLoadout::ZHitman5_Activate);
}

void StartingLoadout::OnDrawMenu()
{
    if (ImGui::Button(ICON_MD_TUNE " Starting loadout"))
    {
        m_ShowWindow = !m_ShowWindow;
    }
}

void StartingLoadout::OnDrawUI(const bool hasFocus)
{
    if (!hasFocus || !m_ShowWindow)
    {
        return;
    }

    ImGui::PushFont(SDK::GetInstance().GetBoldFont());
    ImGui::SetNextWindowSize(ImVec2(500, 300), ImGuiCond_FirstUseEver);

    const bool isWindowExpanded = ImGui::Begin(ICON_MD_TUNE " Starting loadout", &m_ShowWindow);

    ImGui::PushFont(SDK::GetInstance().GetRegularFont());

    if (isWindowExpanded)
    {
        if (m_OutfitNameToTokenID.size() == 0)
        {
            LoadOutfits();
        }

        if (m_FirearmNameToTokenID.size() == 0)
        {
            LoadFirearms();
        }

        static char outfit[1024]{};
        static char firearm[1024]{};

        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Oufit");

        ImGui::SameLine();

        util::InputWithAutocomplete(
            "##Outfit", outfit, sizeof(outfit), m_OutfitNameToTokenID, [](const auto& p_Pair) -> const std::string& { return p_Pair.first; },
            [](const auto& p_Pair) -> const std::string& { return p_Pair.first; },
            [&](const std::string&, const std::string& p_Name, const auto& p_Pair) { m_SelectedOutfit = p_Pair.second; }
        );

        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Firearm");

        ImGui::SameLine();

        util::InputWithAutocomplete(
            "##Firearm", firearm, sizeof(firearm), m_FirearmNameToTokenID, [](const auto& p_Pair) -> std::string { return p_Pair.first; },
            [](const auto& p_Pair) -> const std::string& { return p_Pair.first; },
            [&](const std::string&, const std::string& p_Name, const auto& p_Pair) { m_SelectedFirearm = p_Pair.second; }
        );
    }

    ImGui::PopFont();
    ImGui::End();
    ImGui::PopFont();
}

void StartingLoadout::LoadOutfits()
{
    for (const auto& entry : Globals::ContentKitManager->m_Outfits.m_List.m_List)
    {
        const ZOutfitKitEntity* outfitKitEntity = static_cast<ZOutfitKitEntity*>(entry.GetFirstKit().m_pInterfaceRef);

        m_OutfitNameToTokenID.insert({ outfitKitEntity->m_sCachedTitle.ToCString(), outfitKitEntity->GetTokenID() });
    }
}

void StartingLoadout::LoadFirearms()
{
    for (const auto& entry : Globals::ContentKitManager->m_FireArms.m_List.m_List)
    {
        const ZFireArmKitEntity* fireArmKitEntity = static_cast<ZFireArmKitEntity*>(entry.GetFirstKit().m_pInterfaceRef);

        m_FirearmNameToTokenID.insert({ fireArmKitEntity->m_sCachedTitle.ToCString(), fireArmKitEntity->GetTokenID() });
    }
}

DEFINE_THISCALL_DETOUR_WITH_CONTEXT(StartingLoadout, void, ZHitman5_Activate, ZHitman5* p_Hitman5, const ZString& p_Subset)
{
    if (m_SelectedOutfit != STokenID::InvalidToken)
    {
        Globals::LevelManager->m_SceneTransitionData.BonusOutfit = m_SelectedOutfit;
    }

    if (m_SelectedFirearm != STokenID::InvalidToken)
    {
        Globals::LevelManager->m_SceneTransitionData.BonusWeapon = m_SelectedFirearm;
    }

    return { HookAction::Continue() };
}

DEFINE_MOD(StartingLoadout);
