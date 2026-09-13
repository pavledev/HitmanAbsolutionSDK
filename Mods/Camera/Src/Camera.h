#pragma once

#include <IModInterface.h>

class ZEntitySceneContext;
class ZString;
class ZRenderPostfilterParametersEntity;
struct SRenderPostfilterParametersColorCorrection;
struct SRenderPostfilterParametersMisc;

class Camera : public IModInterface
{
  public:
    void Initialize() override;
    void OnDrawMenu() override;
    void OnDrawUI(const bool hasFocus) override;

    DECLARE_THISCALL_DETOUR_WITH_CONTEXT(Camera, void, ZCameraEntity_SetFovYDeg, ZCameraEntity* p_CameraEntity, float p_FovYDeg);
    DECLARE_THISCALL_DETOUR_WITH_CONTEXT(
        Camera, void, ZEntitySceneContext_CreateScene, ZEntitySceneContext* p_EntitySceneContext, const ZString& p_StreamingState
    );
    DECLARE_THISCALL_DETOUR_WITH_CONTEXT(
        Camera, void, ZRenderPostfilterParametersEntity_UpdateParametersColorCorrection,
        ZRenderPostfilterParametersEntity* p_RenderPostfilterParametersEntity, SRenderPostfilterParametersColorCorrection* p_Parameters,
        SRenderPostfilterParametersMisc* p_MiscParams
    );

  private:
    static bool SliderFloatWithSteps(const char* p_Label, float* p_Value, float p_Min, float p_Max, float p_Step, const char* p_Format);

    bool m_ShowWindow = false;

    float m_FOV;
    bool m_ColorCorrectionEnabled = true;
    bool m_DepthRemapEnabled = true;
    bool m_VignetteEnabled = true;
};

DECLARE_MOD(Camera)
