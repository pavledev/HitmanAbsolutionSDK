#include <imgui.h>

#include <IconsMaterialDesign.h>

#include "MainMenu.h"
#include "ModSelector.h"
#include "ModSDK.h"

namespace UI
{
    void MainMenu::Draw(IImGuiRenderer* p_Renderer, bool p_HasFocus)
    {
        if (!p_HasFocus)
        {
            return;
        }

        ImGui::BeginMainMenuBar();

        ImGui::PushFont(p_Renderer->GetBoldFont());
        ImGui::Text("Hitman Absolution SDK");
        ImGui::PopFont();

        if (ImGui::Button(ICON_MD_TOKEN " MODS"))
        {
            ModSDK::GetInstance().GetUIModSelector()->Show();
        }

        ModSDK::GetInstance().OnDrawMenu();

        ImGui::EndMainMenuBar();
    }
}
