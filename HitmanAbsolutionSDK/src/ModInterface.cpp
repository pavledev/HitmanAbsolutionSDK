#include "imgui.h"

#include "ModInterface.h"
#include "SDK.h"

void ModInterface::SetupUI()
{
    SDK& sdk = SDK::GetInstance();
    ImGuiContext* imGuiContext = sdk.GetImGuiContext();

    if (!imGuiContext)
    {
        return;
    }

    ImGui::SetCurrentContext(imGuiContext);
    ImGui::SetAllocatorFunctions(sdk.GetImGuiMemAllocFunc(), sdk.GetImGuiMemFreeFunc(), sdk.GetImGuiUserDataAllocator());
}
