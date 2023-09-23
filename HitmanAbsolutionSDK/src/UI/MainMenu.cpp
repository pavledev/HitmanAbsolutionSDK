#include <imgui.h>

#include <IconsMaterialDesign.h>

#include <UI/MainMenu.h>
#include <SDK.h>

void MainMenu::Draw(const bool hasFocus)
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

    SDK::GetInstance().OnDrawMenu();

    ImGui::EndMainMenuBar();
}
