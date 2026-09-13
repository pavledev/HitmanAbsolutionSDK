#include <imgui.h>

#include <IconsMaterialDesign.h>

#include "MainMenu.h"
#include "ModSelector.h"
#include "Settings.h"
#include <SDK.h>

namespace UI
{
    void MainMenu::Draw(bool hasFocus)
    {
        if (!hasFocus)
        {
            return;
        }

        ImGui::BeginMainMenuBar();

        ImGui::PushFont(SDK::GetInstance().GetBoldFont());
        ImGui::Text("Hitman Absolution SDK");
        ImGui::PopFont();

        if (ImGui::Button(ICON_MD_TOKEN " MODS"))
        {
            SDK::GetInstance().GetModSelector()->Show();
        }

        if (ImGui::Button(ICON_MD_SETTINGS " Settings"))
        {
            SDK::GetInstance().GetSettings()->Show();
        }

        SDK::GetInstance().OnDrawMenu();

        ImGui::EndMainMenuBar();
    }
}
