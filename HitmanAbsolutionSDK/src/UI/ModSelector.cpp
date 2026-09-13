#include <imgui.h>

#include <IconsMaterialDesign.h>

#include "ModSelector.h"
#include <SDK.h>

namespace UI
{
    ModSelector::ModSelector()
    {
        m_ShowWindow = false;

        InitializeSRWLock(&srwLock);
    }

    void ModSelector::Draw(const bool hasFocus)
    {
        if (!hasFocus || !m_ShowWindow)
        {
            return;
        }

        ImGui::PushFont(SDK::GetInstance().GetBoldFont());
        ImGui::SetNextWindowSize(ImVec2(600, 500), ImGuiCond_FirstUseEver);

        const bool isWindowVisible = ImGui::Begin(ICON_MD_TOKEN " Mods", &m_ShowWindow, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);

        ImGui::PushFont(SDK::GetInstance().GetRegularFont());

        if (isWindowVisible)
        {
            ImGui::TextUnformatted("Select mods:");
            ImGui::Separator();

            ImGui::BeginChild("ScrollingRegion", ImVec2(0, 300), false, ImGuiWindowFlags_HorizontalScrollbar);

            AcquireSRWLockShared(&srwLock);

            for (Mod& mod : availableMods)
            {
                ImGui::Checkbox(mod.m_Name.c_str(), &mod.m_IsEnabled);
            }

            ReleaseSRWLockShared(&srwLock);

            ImGui::EndChild();
            ImGui::Separator();

            if (ImGui::Button("Ok"))
            {
                EnableSelectedMods();

                m_ShowWindow = false;
            }

            if (ImGui::Button("Refresh List"))
            {
                SDK::GetInstance().GetModManager()->FindAvailableMods();
            }
        }

        ImGui::PopFont();
        ImGui::End();
        ImGui::PopFont();
    }

    void ModSelector::Show()
    {
        m_ShowWindow = true;
    }

    void ModSelector::EnableSelectedMods()
    {
        std::set<std::string> mods;

        for (auto& mod : availableMods)
        {
            if (mod.m_IsEnabled)
            {
                mods.insert(mod.m_Name);
            }
        }

        SDK::GetInstance().GetModManager()->SetEnabledMods(mods);
    }

    void ModSelector::UpdateAvailableMods(const std::set<std::string>& p_Mods, const std::set<std::string>& p_ActiveMods)
    {
        ScopedExclusiveGuard guard = ScopedExclusiveGuard(&srwLock);

        availableMods.clear();

        for (const std::string& mod : p_Mods)
        {
            availableMods.push_back({ mod, p_ActiveMods.contains(mod) });
        }
    }
}
