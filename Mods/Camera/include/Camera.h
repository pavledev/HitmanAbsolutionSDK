#pragma once

#include <ModInterface.h>

class ZHM5MainCamera;
struct SGameUpdateEvent;
class ZEntitySceneContext;
class ZRenderPostfilterParametersEntity;
struct SRenderPostfilterParametersColorCorrection;
struct SRenderPostfilterParametersMisc;

void __fastcall ZCameraEntity_SetFovYDegHook(ZCameraEntity* pThis, int edx, float fFovYDeg);
void __fastcall ZEntitySceneContext_CreateSceneHook(ZEntitySceneContext* pThis, int edx, const ZString& sStreamingState);
void __fastcall ZRenderPostfilterParametersEntity_UpdateParametersColorCorrectionHook(
	ZRenderPostfilterParametersEntity* pThis,
	int edx,
	SRenderPostfilterParametersColorCorrection* parameters,
	SRenderPostfilterParametersMisc* miscParams);

class Camera : public ModInterface
{
public:
	void Initialize() override;
	void OnDrawMenu() override;
	void OnDrawUI(const bool hasFocus) override;

	void OnSetFovYDeg(ZCameraEntity* cameraEntity, float fFovYDeg);
	void OnCreateScene(ZEntitySceneContext* entitySceneContext, const ZString& streamingState);
	void OnUpdateParametersColorCorrection(ZRenderPostfilterParametersEntity* renderPostfilterParametersEntity);

private:
	static bool SliderFloatWithSteps(const char* label, float* v, float v_min, float v_max, float v_step, const char* format);

	bool isOpen;

	float fov;
	bool areLUTAndVignetteEffectsEnabled = true;
};

DECLARE_MOD(Camera)
