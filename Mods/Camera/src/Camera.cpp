#include <IconsMaterialDesign.h>

#include "imgui.h"

#include "Glacier/Camera/ZHM5MainCamera.h"
#include "Glacier/ZLevelManager.h"
#include "Glacier/Render/ZRenderPostfilterControllerEntity.h"
#include "Glacier/Render/ZRenderPostfilterParametersEntity.h"
#include "Glacier/Render/SRenderPostfilterParametersColorCorrection.h"

#include "Camera.h"
#include "Hooks.h"
#include <imgui_internal.h>

void Camera::Initialize()
{
    ModInterface::Initialize();

    Hooks::ZHM5MainCamera_UpdateMainCamera.CreateHook("ZHM5MainCamera::UpdateMainCamera", 0x7E160, ZHM5MainCamera_UpdateMainCameraHook);
    Hooks::ZEntitySceneContext_CreateScene.CreateHook("ZEntitySceneContext::CreateScene", 0x4479E0, ZEntitySceneContext_CreateSceneHook);
    Hooks::ZRenderPostfilterParametersEntity_UpdateParametersColorCorrection.CreateHook("ZRenderPostfilterParametersEntity::UpdateParametersColorCorrection", 0x46570, ZRenderPostfilterParametersEntity_UpdateParametersColorCorrectionHook);

    Hooks::ZHM5MainCamera_UpdateMainCamera.EnableHook();
    Hooks::ZEntitySceneContext_CreateScene.EnableHook();
    Hooks::ZRenderPostfilterParametersEntity_UpdateParametersColorCorrection.EnableHook();
}

void Camera::OnDrawMenu()
{
    if (ImGui::Button(ICON_MD_CAMERA " Camera"))
    {
        isOpen = !isOpen;
    }
}

void Camera::OnDrawUI(const bool hasFocus)
{
    if (!hasFocus || !isOpen)
    {
        return;
    }

    ImGui::PushFont(SDK::GetInstance().GetBoldFont());
    ImGui::SetNextWindowSize(ImVec2(1250, 850), ImGuiCond_FirstUseEver);

    const bool isWindowVisible = ImGui::Begin(ICON_MD_CAMERA " Camera", &isOpen, ImGuiWindowFlags_NoScrollbar);

    ImGui::PushFont(SDK::GetInstance().GetRegularFont());

    if (isWindowVisible)
    {
        ZHitman5* hitman = LevelManager->GetHitman().GetRawPointer();
        ZHM5MainCamera* mainCamera = nullptr;

        if (hitman)
        {
            mainCamera = hitman->GetMainCamera();
        }

        if (mainCamera && fov == 0)
        {
            fov = mainCamera->GetFovYDeg();
        }

        SliderFloatWithSteps("Field Of View", &fov, 1.f, 90.f, 1.f, nullptr);

        if (ImGui::Checkbox("LUT And Vignette Effects", &areLUTAndVignetteEffectsEnabled))
        {
            if (mainCamera)
            {
                ZRenderPostfilterControllerEntity* renderPostfilterControllerEntity = static_cast<ZRenderPostfilterControllerEntity*>(mainCamera->GetRenderPostfilterControllerEntity().GetRawPointer());
                ZEntityRef currentPostfilterParametersEntity = renderPostfilterControllerEntity->GetCurrentParametersEntity().GetEntityRef();

                currentPostfilterParametersEntity.SetProperty("m_bColorCorrectionEnabled", areLUTAndVignetteEffectsEnabled);
                currentPostfilterParametersEntity.SetProperty("m_bDepthRemapEnabled", areLUTAndVignetteEffectsEnabled);
                currentPostfilterParametersEntity.SetProperty("m_bVignetteEnabled", areLUTAndVignetteEffectsEnabled);
            }
        }

        ImGui::Separator();

        if (ImGui::Button("Ok"))
        {
            isOpen = false;
        }
    }

    ImGui::PopFont();
    ImGui::End();
    ImGui::PopFont();
}

void Camera::OnUpdateMainCamera(ZHM5MainCamera* mainCamera)
{
    if (fov > 0)
    {
        mainCamera->SetFovYDeg(fov);
    }
}

void Camera::OnCreateScene(ZEntitySceneContext* entitySceneContext, const ZString& streamingState)
{
    ZHitman5* hitman = LevelManager->GetHitman().GetRawPointer();

    if (!hitman)
    {
        return;
    }

    ZHM5MainCamera* mainCamera = hitman->GetMainCamera();
    ZRenderPostfilterControllerEntity* renderPostfilterControllerEntity = static_cast<ZRenderPostfilterControllerEntity*>(mainCamera->GetRenderPostfilterControllerEntity().GetRawPointer());
    ZEntityRef currentPostfilterParametersEntity = renderPostfilterControllerEntity->GetCurrentParametersEntity().GetEntityRef();

    currentPostfilterParametersEntity.SetProperty("m_bColorCorrectionEnabled", areLUTAndVignetteEffectsEnabled);
    currentPostfilterParametersEntity.SetProperty("m_bDepthRemapEnabled", areLUTAndVignetteEffectsEnabled);
    currentPostfilterParametersEntity.SetProperty("m_bVignetteEnabled", areLUTAndVignetteEffectsEnabled);
}

void Camera::OnUpdateParametersColorCorrection(ZRenderPostfilterParametersEntity* renderPostfilterParametersEntity)
{
    const bool isColorCorrectionEnabled = renderPostfilterParametersEntity->GetID().GetProperty("m_bColorCorrectionEnabled").Get<bool>();

    if (isColorCorrectionEnabled && !areLUTAndVignetteEffectsEnabled)
    {
        renderPostfilterParametersEntity->GetID().SetProperty("m_bColorCorrectionEnabled", areLUTAndVignetteEffectsEnabled);
        renderPostfilterParametersEntity->GetID().SetProperty("m_bDepthRemapEnabled", areLUTAndVignetteEffectsEnabled);
        renderPostfilterParametersEntity->GetID().SetProperty("m_bVignetteEnabled", areLUTAndVignetteEffectsEnabled);
    }
}

bool Camera::SliderFloatWithSteps(const char* label, float* v, float v_min, float v_max, float v_step, const char* format)
{
    if (!format)
    {
        format = "%.3f";
    }

    char textBuffer[64] = {};

    ImFormatString(textBuffer, IM_ARRAYSIZE(textBuffer), format, *v);

    // Map from [v_min,v_max] to [0,N]
    const int countValues = int((v_max - v_min) / v_step);
    int v_i = int((*v - v_min) / v_step);
    const bool valueChanged = ImGui::SliderInt(label, &v_i, 0, countValues, textBuffer);

    // Remap from [0,N] to [v_min,v_max]
    *v = v_min + float(v_i) * v_step;

    return valueChanged;
}

void __fastcall ZHM5MainCamera_UpdateMainCameraHook(ZHM5MainCamera* pThis, int edx, const SGameUpdateEvent* updateEvent, bool bPaused)
{
    Hooks::ZHM5MainCamera_UpdateMainCamera.CallOriginalFunction(pThis, updateEvent, bPaused);

    GetModInstance()->OnUpdateMainCamera(pThis);
}

void __fastcall ZEntitySceneContext_CreateSceneHook(ZEntitySceneContext* pThis, int edx, const ZString& sStreamingState)
{
    Hooks::ZEntitySceneContext_CreateScene.CallOriginalFunction(pThis, sStreamingState);

    GetModInstance()->OnCreateScene(pThis, sStreamingState);
}

void __fastcall ZRenderPostfilterParametersEntity_UpdateParametersColorCorrectionHook(ZRenderPostfilterParametersEntity* pThis, int edx, SRenderPostfilterParametersColorCorrection* parameters, SRenderPostfilterParametersMisc* miscParams)
{
    GetModInstance()->OnUpdateParametersColorCorrection(pThis);

    Hooks::ZRenderPostfilterParametersEntity_UpdateParametersColorCorrection.CallOriginalFunction(pThis, parameters, miscParams);
}

DEFINE_MOD(Camera);
