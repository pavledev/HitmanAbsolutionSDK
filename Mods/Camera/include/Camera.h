#pragma once

#include <ModInterface.h>

class ZHM5MainCamera;
struct SGameUpdateEvent;
class ZEntitySceneContext;

void __fastcall ZHM5MainCamera_UpdateMainCameraHook(ZHM5MainCamera* pThis, int edx, const SGameUpdateEvent* updateEvent, bool bPaused);
void __fastcall ZEntitySceneContext_CreateSceneHook(ZEntitySceneContext* pThis, int edx, const ZString& sStreamingState);

class Camera : public ModInterface
{
public:
	void Initialize() override;
	void OnDrawMenu() override;
	void OnDrawUI(const bool hasFocus) override;

	void OnUpdateMainCamera(ZHM5MainCamera* mainCamera);
	void OnCreateScene(ZEntitySceneContext* entitySceneContext, const ZString& streamingState);

private:
	static bool SliderFloatWithSteps(const char* label, float* v, float v_min, float v_max, float v_step, const char* format);

	bool isOpen;

	float fov;
	bool areLUTAndVignetteEffectsEnabled = true;
};

DECLARE_MOD(Camera)
