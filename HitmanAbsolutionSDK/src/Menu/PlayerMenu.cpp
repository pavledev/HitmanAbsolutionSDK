#include "imgui.h"

#include "Menu/PlayerMenu.h"

void PlayerMenu::Render(bool* open)
{
	ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_FirstUseEver);

	if (!ImGui::Begin("Player Info", open))
	{
		ImGui::End();

		return;
	}

	ImGui::End();
}
