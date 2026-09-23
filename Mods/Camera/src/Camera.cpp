#include "Camera.h"

#include <IconsMaterialDesign.h>

#include "imgui.h"

#include "Glacier/ZCamera.h"
#include "Glacier/ZLevelManager.h"
#include "Glacier/ZRender.h"

#include "Hooks.h"
#include <imgui_internal.h>

void Camera::Initialize()
{
    Hooks::ZEntitySceneContext_CreateScene->AddDetour(this, &Camera::ZEntitySceneContext_CreateScene);
    Hooks::ZRenderPostfilterParametersEntity_UpdateParametersColorCorrection->AddDetour(
        this, &Camera::ZRenderPostfilterParametersEntity_UpdateParametersColorCorrection
    );
    Hooks::ZCameraEntity_SetFovYDeg->AddDetour(this, &Camera::ZCameraEntity_SetFovYDeg);
}

void Camera::OnDrawMenu(IImGuiRenderer* p_Renderer)
{
    if (ImGui::Button(ICON_MD_CAMERA " Camera"))
    {
        m_ShowWindow = !m_ShowWindow;
    }
}

void Camera::OnDrawUI(IImGuiRenderer* p_Renderer, bool p_HasFocus)
{
    if (!p_HasFocus || !m_ShowWindow)
    {
        return;
    }

    ImGui::PushFont(p_Renderer->GetBlackFont());
    ImGui::SetNextWindowSize(ImVec2(500, 300), ImGuiCond_FirstUseEver);

    const bool isWindowExpanded = ImGui::Begin(ICON_MD_CAMERA " Camera", &m_ShowWindow);

    ImGui::PushFont(p_Renderer->GetRegularFont());

    if (isWindowExpanded)
    {
        ZHitman5* hitman = Globals::LevelManager->m_rHitman.m_pInterfaceRef;
        ZHM5MainCamera* mainCamera = nullptr;

        if (hitman)
        {
            mainCamera = hitman->m_rMainCamera.m_pInterfaceRef;
        }

        if (mainCamera && m_FOV == 0)
        {
            m_FOV = mainCamera->GetFovYDeg();
        }

        ImGui::BeginDisabled(!mainCamera);

        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Field of view");

        ImGui::SameLine();

        SliderFloatWithSteps("##FieldOfView", &m_FOV, 1.f, 90.f, 1.f, nullptr);

        ImGui::Separator();

        ZEntityRef currentPostfilterParametersEntity;

        if (mainCamera && mainCamera->m_rPostfilter.m_pInterfaceRef)
        {
            const auto renderPostfilterControllerEntity = static_cast<ZRenderPostfilterControllerEntity*>(mainCamera->m_rPostfilter.m_pInterfaceRef);
            currentPostfilterParametersEntity = renderPostfilterControllerEntity->GetCurrentParametersEntity().m_entityRef;
        }

        ImGui::TextUnformatted("Effects");
        ImGui::Spacing();

        if (ImGui::Checkbox("Color correction", &m_ColorCorrectionEnabled))
        {
            currentPostfilterParametersEntity.SetProperty("m_bColorCorrectionEnabled", m_ColorCorrectionEnabled);
        }

        if (ImGui::Checkbox("Depth remap", &m_DepthRemapEnabled))
        {
            currentPostfilterParametersEntity.SetProperty("m_bDepthRemapEnabled", m_DepthRemapEnabled);
        }

        if (ImGui::Checkbox("Vignette", &m_VignetteEnabled))
        {
            currentPostfilterParametersEntity.SetProperty("m_bVignetteEnabled", m_VignetteEnabled);
        }

        ImGui::EndDisabled();
    }

    ImGui::PopFont();
    ImGui::End();
    ImGui::PopFont();
}

DEFINE_THISCALL_MOD_DETOUR(Camera, void, ZCameraEntity_SetFovYDeg, ZCameraEntity* p_CameraEntity, float p_FovYDeg)
{
    if (m_FOV > 0)
    {
        p_FovYDeg = m_FOV;
    }

    p_CameraEntity->SetFovYDeg(p_FovYDeg);

    return { HookAction::Return() };
}

DEFINE_THISCALL_MOD_DETOUR(Camera, void, ZEntitySceneContext_CreateScene, ZEntitySceneContext* p_EntitySceneContext, const ZString& p_StreamingState)
{
    p_Hook->CallOriginal(p_EntitySceneContext, p_StreamingState);

    ZHitman5* hitman = Globals::LevelManager->m_rHitman.m_pInterfaceRef;

    if (!hitman)
    {
        return { HookAction::Return() };
    }

    ZHM5MainCamera* mainCamera = hitman->m_rMainCamera.m_pInterfaceRef;
    const auto renderPostfilterControllerEntity = static_cast<ZRenderPostfilterControllerEntity*>(mainCamera->m_rPostfilter.m_pInterfaceRef);
    ZEntityRef currentPostfilterParametersEntity = renderPostfilterControllerEntity->GetCurrentParametersEntity().m_entityRef;

    currentPostfilterParametersEntity.SetProperty("m_bColorCorrectionEnabled", m_ColorCorrectionEnabled);
    currentPostfilterParametersEntity.SetProperty("m_bDepthRemapEnabled", m_DepthRemapEnabled);
    currentPostfilterParametersEntity.SetProperty("m_bVignetteEnabled", m_VignetteEnabled);

    return { HookAction::Return() };
}

DEFINE_THISCALL_MOD_DETOUR(
    Camera, void, ZRenderPostfilterParametersEntity_UpdateParametersColorCorrection,
    ZRenderPostfilterParametersEntity* p_RenderPostfilterParametersEntity, SRenderPostfilterParametersColorCorrection* p_Parameters,
    SRenderPostfilterParametersMisc* p_MiscParams
)
{
    ZEntityRef entityRef = p_RenderPostfilterParametersEntity->GetID();

    const bool isColorCorrectionEnabled = entityRef.GetProperty("m_bColorCorrectionEnabled").Get<bool>();
    const bool isDepthRemapEnabled = entityRef.GetProperty("m_bDepthRemapEnabled").Get<bool>();
    const bool isVignetteEnabled = entityRef.GetProperty("m_bVignetteEnabled").Get<bool>();

    if (isColorCorrectionEnabled && !m_ColorCorrectionEnabled)
    {
        entityRef.SetProperty("m_bColorCorrectionEnabled", m_ColorCorrectionEnabled);
    }

    if (isDepthRemapEnabled && !m_DepthRemapEnabled)
    {
        entityRef.SetProperty("m_bDepthRemapEnabled", m_DepthRemapEnabled);
    }

    if (isVignetteEnabled && !m_VignetteEnabled)
    {
        entityRef.SetProperty("m_bVignetteEnabled", m_VignetteEnabled);
    }

    return { HookAction::Continue() };
}

bool Camera::SliderFloatWithSteps(const char* p_Label, float* p_Value, float p_Min, float p_Max, float p_Step, const char* p_Format)
{
    if (!p_Format)
    {
        p_Format = "%.3f";
    }

    char textBuffer[64] = {};

    ImFormatString(textBuffer, IM_ARRAYSIZE(textBuffer), p_Format, *p_Value);

    const int32_t stepCount = static_cast<int32_t>((p_Max - p_Min) / p_Step);
    int32_t stepIndex = static_cast<int32_t>(std::round((*p_Value - p_Min) / p_Step));

    const bool valueChanged = ImGui::SliderInt(p_Label, &stepIndex, 0, stepCount, textBuffer);

    *p_Value = p_Min + static_cast<float>(stepIndex) * p_Step;

    return valueChanged;
}

DEFINE_HMASDK_MOD(Camera);
