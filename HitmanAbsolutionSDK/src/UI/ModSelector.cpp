#include <imgui.h>

#include <IconsMaterialDesign.h>

#include "ModSelector.h"
#include "ModSDK.h"
#include <Utils/StringUtils.h>

namespace UI
{
    ModSelector::ModSelector()
    {
        InitializeSRWLock(&m_Lock);
    }

    void ModSelector::UpdateAvailableMods(
        const std::unordered_set<std::string>& p_Mods, const std::unordered_set<std::string>& p_IncompatibleMods,
        const std::unordered_set<std::string>& p_ActiveMods
    )
    {
        ScopedExclusiveGuard guard(&m_Lock);

        m_IncompatibleMods = p_IncompatibleMods;

        m_AvailableMods.clear();

        for (auto& mod : p_Mods)
        {
            m_AvailableMods.push_back({ mod, p_ActiveMods.contains(util::ToLowerCase(mod)) });
        }

        std::ranges::sort(m_AvailableMods, [](const auto& a, const auto& b) { return util::ToLowerCase(a.m_Name) < util::ToLowerCase(b.m_Name); });

        // If there are no active mods then set this flag which will make it
        // so the mod selector is shown automatically when a user launches the game.
        if (p_ActiveMods.empty())
        {
            m_ShouldShowWindow = true;
        }
        else
        {
            m_ShouldShowWindow = false;
        }
    }

    void ModSelector::Draw(IImGuiRenderer* p_Renderer, bool p_HasFocus)
    {
        if (m_ShouldShowWindow)
        {
            m_Open = true;
            m_ShouldShowWindow = false;
            SDK().RequestUIFocus();
        }

        if (!m_Open || !p_HasFocus)
        {
            return;
        }

        const auto wasOpen = m_Open;

        ImGui::PushFont(p_Renderer->GetBlackFont());

        const auto isWindowExpanded = ImGui::Begin(ICON_MD_TOKEN " MODS", &m_Open, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);

        ImGui::PushFont(p_Renderer->GetRegularFont());

        if (isWindowExpanded)
        {
            ImGui::TextUnformatted(
                "Select the mods you'd like to use.\nKeep in mind that some mods might require a game restart to function properly."
            );
            ImGui::Separator();

            ImGui::BeginChild("ScrollingRegion", ImVec2(0, 300), false, ImGuiWindowFlags_HorizontalScrollbar);

            AcquireSRWLockShared(&m_Lock);

            for (auto& mod : m_AvailableMods)
            {
                ImGui::Checkbox(mod.m_Name.c_str(), &mod.m_IsEnabled);
            }

            if (m_IncompatibleMods.size() > 0)
            {
                ImGui::Separator();
                ImGui::PushFont(p_Renderer->GetBlackFont());
                ImGui::TextColored(ImVec4(1.0, 0.0, 0.0, 1.0), "NOTICE");
                ImGui::PopFont();
                ImGui::Text("The following mods are incompatible and need");
                ImGui::Text("to be updated before they can be enabled.");
                ImGui::Spacing();
            }

            for (auto& mod : m_IncompatibleMods)
            {
                ImGui::BeginDisabled(true);
                bool off = false;
                ImGui::Checkbox(mod.c_str(), &off);
                ImGui::EndDisabled();
            }

            ReleaseSRWLockShared(&m_Lock);

            ImGui::EndChild();

            ImGui::Separator();

            if (ImGui::Button("OK"))
            {
                ApplySelectedMods();
                m_Open = false;
            }

            if (ImGui::Button("Refresh"))
            {
                ModSDK::GetInstance().GetModManager()->FindAvailableMods();
            }
        }

        ImGui::PopFont();
        ImGui::End();
        ImGui::PopFont();

        // If a user closed this, then release focus.
        if (wasOpen && !m_Open)
        {
            SDK().ReleaseUIFocus();
        }
    }

    void ModSelector::ApplySelectedMods()
    {
        std::unordered_set<std::string> mods;

        for (auto& mod : m_AvailableMods)
        {
            if (mod.m_IsEnabled)
            {
                mods.insert(mod.m_Name);
            }
        }

        ModSDK::GetInstance().GetModManager()->SetActiveMods(mods);
    }
}
