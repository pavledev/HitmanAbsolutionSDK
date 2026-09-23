#pragma once

#include <string>
#include <unordered_map>

#include <Glacier/ZGameLoopManager.h>
#include <Glacier/ZEntity.h>
#include <Glacier/ZInput.h>
#include <Glacier/ZPhysics.h>

#include <IModInterface.h>

class ZEntitySceneContext;
class ZString;
class ZFreeCameraControlEntity;

class FreeCamera : public IModInterface
{
  public:
    FreeCamera();
    ~FreeCamera() override;

    void Initialize() override;
    void OnEngineInitialized() override;
    void OnDrawMenu(IImGuiRenderer* p_Renderer) override;
    void OnDrawUI(IImGuiRenderer* p_Renderer, bool p_HasFocus) override;

  private:
    void OnFrameUpdate(const SGameUpdateEvent& updateEvent);

    void ToggleFreeCamera();
    void EnableFreeCamera();
    void DisableFreeCamera();
    void TogglePlayerInput();
    void SetFreeCamFrozen(bool p_Frozen);

    void UpdateFov(const float p_Delta);
    static float
    MoveValueWithinRange(const float p_Delta, const float p_CurrentValue, const float p_PivotValue, const float p_RangeMin, const float p_RangeMax);

    bool RaycastFromFreeCamera(ZRayQueryOutput& p_RayQueryOutput);

    void TeleportPlayer();

    void KillActor();

    DECLARE_THISCALL_MOD_DETOUR(FreeCamera, void, ZEntitySceneContext_ClearScene, ZEntitySceneContext* p_EntitySceneContext, bool p_FullyUnloadScene);
    DECLARE_THISCALL_MOD_DETOUR(FreeCamera, void, ZEngineAppCommon_ResetSceneCallback, ZEngineAppCommon* p_EngineAppCommon);

    DECLARE_THISCALL_MOD_DETOUR(
        FreeCamera, void, ZFreeCameraControlEntity_UpdateCamera, ZFreeCameraControlEntity* p_FreeCameraControlEntity, float p_Dt
    );
    DECLARE_THISCALL_MOD_DETOUR(
        FreeCamera, void, ZFreeCameraControlEntity_UpdateMovementFromInput, ZFreeCameraControlEntity* p_FreeCameraControlEntity
    );

    bool m_IsFreeCameraActive;
    bool m_ShouldToggle;
    bool m_IsGamePaused;
    bool m_IsPlayerInputEnabled;
    bool m_IsFreeCameraFrozen;

    ZInputAction m_ToggleFreeCameraAction;
    ZInputAction m_ActivatePlayerInputAction;
    ZInputAction m_ActivateGameControlAction;
    ZInputAction m_TogglePauseGameAction;
    ZInputAction m_TeleportPlayerAction;
    ZInputAction m_KillActorAction;

    bool m_ShowFreeCameraWindow;
    bool m_ShowControlsWindow;
    std::unordered_map<std::string, std::string> m_PcControls;
    std::unordered_map<std::string, std::string> m_ControllerControls;

    float m_FreeCamSpeedChangeThreshold;
    float m_FreeCamTranslationSpeedChangeSensitivity;
    float m_FreeCamTranslationSpeedMin;
    float m_FreeCamTranslationSpeedMax;
    float m_FreeCamRotationSpeedMin;
    float m_FreeCamRotationSpeedMax;
    float m_FreeCamRotationSpeedChangeSensitivity;
    float m_FreeCamFovDependentSpeedMin;
    float m_FreeCamFovDependentSpeedMax;

    float m_FOVDependentSpeedMultiplier;
    float m_TemporaryTranslationSpeedMultiplier;
    float m_PersistentTranslationSpeedMultiplier;
    float m_DeltaTranslationSpeed;
    float m_TemporaryRotationSpeedMultiplier;
    float m_PersistentRotationSpeedMultiplier;
    float m_DeltaRotationSpeed;

    inline static ZInputAction m_LeftBumperAction[] = { "LeftBumper0", "LeftBumper1" };
    inline static ZInputAction m_RightBumperAction[] = { "RightBumper0", "RightBumper1" };
    inline static ZInputAction m_AnalogLeftXAction[] = { "XAxisLeft0", "XAxisLeft1" };
    inline static ZInputAction m_AnalogLeftYAction[] = { "YAxisLeft0", "YAxisLeft1" };
    inline static ZInputAction m_AnalogRightXAction[] = { "XAxisRight0", "XAxisRight1" };
    inline static ZInputAction m_AnalogRightYAction[] = { "YAxisRight0", "YAxisRight1" };
    inline static ZInputAction m_MoveXAction{ "MoveX" };
    inline static ZInputAction m_MoveYAction{ "MoveY" };
    inline static ZInputAction m_MoveZAction{ "MoveZ" };
    inline static ZInputAction m_TiltCameraAction{ "TiltCamera" };
    inline static ZInputAction m_TurnCameraAction{ "TurnCamera" };
    inline static ZInputAction m_LeftTriggerAction[] = { "LeftTrigger0", "LeftTrigger1" };
    inline static ZInputAction m_RightTriggerAction[] = { "RightTrigger0", "RightTrigger1" };
    inline static ZInputAction m_RollModifierAction[] = { "RollModifier0", "RollModifier1" };
    inline static ZInputAction m_FOVModifierAction[] = { "FovModifier0", "FovModifier1" };
    inline static ZInputAction m_ResetRollAction[] = { "ResetRoll0", "ResetRoll1" };
    inline static ZInputAction m_ResetFovAction[] = { "ResetFov0", "ResetFov1" };
    inline static ZInputAction m_SpeedModifierAction[2] = { "SpeedModifier0", "SpeedModifier1" };
    inline static ZInputAction m_ResetSpeedAction[2] = { "ResetSpeed0", "ResetSpeed1" };
};

DECLARE_HMASDK_MOD(FreeCamera)
