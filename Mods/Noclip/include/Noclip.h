#pragma once

#include <Glacier/Scene/ZEntitySceneContext.h>
#include <Glacier/SGameUpdateEvent.h>
#include <Glacier/Input/ZInputAction.h>

#include <ModInterface.h>

void __fastcall ZEntitySceneContext_ClearSceneHook(ZEntitySceneContext* pThis, int edx, bool bFullyUnloadScene);

class Noclip : public ModInterface
{
public:
	Noclip();
	~Noclip();

	void Initialize() override;
	void OnEngineInitialized() override;
	void OnDrawMenu() override;

	void OnClearScene(ZEntitySceneContext* entitySceneContext, bool fullyUnloadScene);

private:
	void OnFrameUpdate(const SGameUpdateEvent& updateEvent);

	bool isNoclipEnabled;
	ZInputAction toggleNoclipAction;
	float4 playerPosition;
};

DECLARE_MOD(Noclip)
