#include <imgui.h>

#include <IconsMaterialDesign.h>

#include <UI/ModSelector.h>
#include <SDK.h>

ModSelector::ModSelector()
{
    isOpen = false;

    InitializeSRWLock(&srwLock);
}

void ModSelector::Draw(const bool hasFocus)
{
    if (!hasFocus || !isOpen)
    {
        return;
    }

    ImGui::PushFont(SDK::GetInstance().GetBoldFont());
    ImGui::SetNextWindowSize(ImVec2(600, 500), ImGuiCond_FirstUseEver);

    const bool isWindowVisible = ImGui::Begin(ICON_MD_TOKEN " Mods", &isOpen, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);

    ImGui::PushFont(SDK::GetInstance().GetRegularFont());

    if (isWindowVisible)
    {
        ImGui::TextUnformatted("Select mods:");
        ImGui::Separator();

        ImGui::BeginChild("ScrollingRegion", ImVec2(0, 300), false, ImGuiWindowFlags_HorizontalScrollbar);

        AcquireSRWLockShared(&srwLock);

        for (Mod& mod : availableMods)
        {
            ImGui::Checkbox(mod.name.c_str(), &mod.isEnabled);
        }

        ReleaseSRWLockShared(&srwLock);

        ImGui::EndChild();
        ImGui::Separator();

        if (ImGui::Button("Ok"))
        {
            EnableSelectedMods();

            isOpen = false;
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
    isOpen = true;
}

void ModSelector::EnableSelectedMods()
{
    std::set<std::string> mods;

    for (auto& mod : availableMods)
    {
        if (mod.isEnabled)
        {
            mods.insert(mod.name);
        }
    }

    SDK::GetInstance().GetModManager()->SetEnabledMods(mods);
}

void ModSelector::UpdateAvailableMods(const std::set<std::string>& mods, const std::set<std::string>& activeMods)
{
    ScopedExclusiveGuard guard = ScopedExclusiveGuard(&srwLock);

    availableMods.clear();

    for (const std::string& mod : mods)
    {
        availableMods.push_back({ mod, activeMods.contains(mod) });
    }
}
