#pragma once

#include <string>
#include <unordered_map>

#include <Glacier/ZGameLoopManager.h>
#include <Glacier/Entity/ZEntityRef.h>
#include <Glacier/Input/ZInputAction.h>
#include <Glacier/Physics/ZRayQueryOutput.h>
#include <Glacier/Physics/ERayDetailLevel.h>

#include <ModInterface.h>

class ZEntitySceneContext;
class ZString;
class ZFreeCameraControlEntity;
class ZInputActionManager;

void __fastcall ZEntitySceneContext_CreateSceneHook(ZEntitySceneContext* pThis, int edx, const ZString& sStreamingState);
void __fastcall ZEntitySceneContext_ClearSceneHook(ZEntitySceneContext* pThis, int edx, bool bFullyUnloadScene);
void __fastcall ZFreeCameraControlEntity_UpdateCameraHook(ZFreeCameraControlEntity* pThis, int edx, float dt);
void __fastcall ZFreeCameraControlEntity_UpdateMovementFromInputHook(ZFreeCameraControlEntity* pThis, int edx);
void __fastcall ZEngineAppCommon_ResetSceneCallbackHook(ZEngineAppCommon* pThis, int edx);

class FreeCamera : public ModInterface
{
public:
    FreeCamera();
    ~FreeCamera() override;

    void Initialize() override;
    void OnEngineInitialized() override;
    void OnDrawMenu() override;
    void OnDrawUI(const bool hasFocus) override;

    void OnCreateScene(ZEntitySceneContext* entitySceneContext, const ZString& streamingState);
    void OnClearScene(ZEntitySceneContext* entitySceneContext, bool fullyUnloadScene);
    void OnUpdateCamera(float delta);
    void OnUpdateMovementFromInput();

private:
    void OnFrameUpdate(const SGameUpdateEvent& updateEvent);
    void ToggleFreeCamera();
    void EnableFreeCamera();
    void DisableFreeCamera();
    
    void UpdateFov(float delta);
    static float MoveValueWithinRange(float delta, float currentValue, float pivotValue, float rangeMin, float rangeMax);

    void InstantlyKillNpc();
    void TeleportMainCharacter();
    bool GetFreeCameraRayCastClosestHitQueryOutput(const ERayDetailLevel detailLevel, ZRayQueryOutput& rayQueryOutput);

    volatile bool isFreeCameraActive;
    volatile bool freezeCamera;
    ZInputAction toggleFreeCameraAction;
    ZInputAction freezeFreeCameraAction;
    ZInputAction speedModifierAction[2];
    ZInputAction resetSpeedAction[2];
    ZInputAction instantlyKillNpcAction;
    ZInputAction teleportMainCharacterAction;
    bool areControlsVisible;
    std::unordered_map<std::string, std::string> pcControls;
    std::unordered_map<std::string, std::string> controllerControls;

    float freeCamSpeedChangeThreshold;
    float freeCamTranslationSpeedChangeSensitivity;
    float freeCamTranslationSpeedMin;
    float freeCamTranslationSpeedMax;
    float freeCamRotationSpeedMin;
    float freeCamRotationSpeedMax;
    float freeCamRotationSpeedChangeSensitivity;
    float freeCamFovDependentSpeedMin;
    float freeCamFovDependentSpeedMax;

    float fovDependentSpeedMultiplier;
    float temporaryTranslationSpeedMultiplier;
    float persistentTranslationSpeedMultiplier;
    float deltaTranslationSpeed;
    float temporaryRotationSpeedMultiplier;
    float persistentRotationSpeedMultiplier;
    float deltaRotationSpeed;

    inline static ZInputAction* leftBumperAction;
    inline static ZInputAction* rightBumperAction;
    inline static ZInputAction* analogLeftXAction;
    inline static ZInputAction* analogLeftYAction;
    inline static ZInputAction* analogRightXAction;
    inline static ZInputAction* analogRightYAction;
    inline static ZInputAction* moveXAction;
    inline static ZInputAction* moveYAction;
    inline static ZInputAction* moveZAction;
    inline static ZInputAction* tiltCameraAction;
    inline static ZInputAction* turnCameraAction;
    inline static ZInputAction* leftTriggerAction;
    inline static ZInputAction* rightTriggerAction;
    inline static ZInputAction* rollModifierAction;
    inline static ZInputAction* fovModifierAction;
    inline static ZInputAction* resetRollAction;
    inline static ZInputAction* resetFovAction;
};

DECLARE_MOD(FreeCamera)
