#include "imgui.h"

#include "IconsMaterialDesign.h"

#include <Glacier/Templates/TEntityRef.h>
#include <Glacier/Render/IRenderDestinationEntity.h>
#include <Glacier/Render/ZRenderManager.h>
#include <Glacier/ZLevelManager.h>
#include <Glacier/Engine/ZApplicationEngineWin32.h>
#include <Glacier/Input/ZInputActionManager.h>
#include <Glacier/Math/ZMath.h>
#include <Glacier/Physics/ZCollisionManager.h>
#include <Glacier/Actor/ZActor.h>
#include <Glacier/UI/ZHUDManager.h>
#include <Glacier/ZGameTimeManager.h>

#include "FreeCamera.h"
#include "Global.h"
#include "SDK.h"
#include "Hooks.h"

FreeCamera::FreeCamera() :
    isFreeCameraActive(false),
    freezeCamera(false),
    toggleFreeCameraAction("ToggleFreeCamera"),
    freezeFreeCameraAction("FreezeCamera"),
    instantlyKillNpcAction("InstantKill"),
    teleportMainCharacterAction("Teleport"),
    pauseGameAction("PauseGame"),
    toggleFreeCameraAndPauseGameAction("ToggleFreeCameraAndPauseGame"),
    areControlsVisible(false),
    pauseGame(false),
    freeCamSpeedChangeThreshold(0.5f),
    deltaTranslationSpeed(0.f),
    freeCamTranslationSpeedChangeSensitivity(4.f),
    freeCamTranslationSpeedMin(0.1f),
    freeCamTranslationSpeedMax(10.f),
    freeCamRotationSpeedMin(0.1f),
    freeCamRotationSpeedMax(2.f),
    freeCamRotationSpeedChangeSensitivity(2.f),
    freeCamFovDependentSpeedMin(0.1f),
    freeCamFovDependentSpeedMax(2.f),
    fovDependentSpeedMultiplier(1.f),
    persistentTranslationSpeedMultiplier(0x3F800000),
    persistentRotationSpeedMultiplier(0x3F800000)
{
    pcControls = {
        { "K", "Toggle freecam" },
        { "F3", "Lock camera and enable 47 input" },
        { "Ctrl + W/S", "Change FOV" },
        { "Ctrl + A/D", "Roll camera" },
        { "Alt + W/S", "Change camera speed" },
        { "Space + Q/E", "Change camera height" },
        { "Space + W/S", "Move camera on axis" },
        { "Shift", "Increase camera speed" },
        { "Ctrl + F9", "Teleport Hitman" },
        { "F9", "Kill NPC" },
    };

    controllerControls = {
        { "Y + L", "Change FOV" },
        { "A + L", "Roll camera" },
        { "A + L press", "Reset rotation" },
        { "B + R", "Change camera speed" },
        { "RT", "Increase camera speed" },
        { "LB", "Lock camera and enable 47 input" },
        { "LT + R", "Change camera height" },
    };
}

FreeCamera::~FreeCamera()
{
    const ZMemberDelegate<FreeCamera, void(const SGameUpdateEvent&)> delegate(this, &FreeCamera::OnFrameUpdate);

    GameLoopManager->UnregisterForFrameUpdate(delegate);

    if (isFreeCameraActive)
    {
        ZApplicationEngineWin32* applicationEngineWin32 = ZApplicationEngineWin32::GetInstance();
        ZEngineAppCommon& engineAppCommon = applicationEngineWin32->GetEngineAppCommon();
        TEntityRef<IRenderDestinationEntity> renderDestinationEntity = RenderManager->GetGameRenderDestinationEntity();

        renderDestinationEntity.GetRawPointer()->SetSource(engineAppCommon.GetMainCamera().GetEntityRef());

        TEntityRef<ZHitman5> hitman = LevelManager->GetHitman();

        if (hitman.GetRawPointer())
        {
            ZHM5InputControl* inputControl = hitman.GetRawPointer()->GetInputControl();

            if (inputControl)
            {
                inputControl->EnableBindings();
            }
        }
    }

    Hooks::ZInputAction_Digital.DisableHook();
    Hooks::ZInputAction_Digital.RemoveHook();

    Hooks::ZEntitySceneContext_CreateScene.DisableHook();
    Hooks::ZEntitySceneContext_CreateScene.RemoveHook();

    Hooks::ZEntitySceneContext_ClearScene.DisableHook();
    Hooks::ZEntitySceneContext_ClearScene.RemoveHook();
}

void FreeCamera::Initialize()
{
    ModInterface::Initialize();

    Hooks::ZEntitySceneContext_CreateScene.CreateHook("ZEntitySceneContext::CreateScene", 0x4479E0, ZEntitySceneContext_CreateSceneHook);
    Hooks::ZEntitySceneContext_ClearScene.CreateHook("ZEntitySceneContext::ClearScene", 0x265A80, ZEntitySceneContext_ClearSceneHook);
    Hooks::ZFreeCameraControlEntity_UpdateCamera.CreateHook("ZFreeCameraControlEntity::UpdateCamera", 0x192990, ZFreeCameraControlEntity_UpdateCameraHook);
    Hooks::ZFreeCameraControlEntity_UpdateMovementFromInput.CreateHook("ZFreeCameraControlEntity::UpdateMovementFromInput", 0x3DFA70, ZFreeCameraControlEntity_UpdateMovementFromInputHook);
    Hooks::ZEngineAppCommon_ResetSceneCallback.CreateHook("ZEngineAppCommon::ResetSceneCallbackHook", 0x53D390, ZEngineAppCommon_ResetSceneCallbackHook);

    Hooks::ZEntitySceneContext_CreateScene.EnableHook();
    Hooks::ZEntitySceneContext_ClearScene.EnableHook();
    Hooks::ZFreeCameraControlEntity_UpdateCamera.EnableHook();
    Hooks::ZFreeCameraControlEntity_UpdateMovementFromInput.EnableHook();
    Hooks::ZEngineAppCommon_ResetSceneCallback.EnableHook();

    leftBumperAction = reinterpret_cast<ZInputAction*>(BaseAddress + 0xE53714);
    rightBumperAction = reinterpret_cast<ZInputAction*>(BaseAddress + 0xE536FC);
    analogLeftXAction = reinterpret_cast<ZInputAction*>(BaseAddress + 0xE5388C);
    analogLeftYAction = reinterpret_cast<ZInputAction*>(BaseAddress + 0xE5375C);
    analogRightXAction = reinterpret_cast<ZInputAction*>(BaseAddress + 0xE53868);
    analogRightYAction = reinterpret_cast<ZInputAction*>(BaseAddress + 0xE53818);
    moveXAction = reinterpret_cast<ZInputAction*>(BaseAddress + 0xE537B0);
    moveYAction = reinterpret_cast<ZInputAction*>(BaseAddress + 0xE53798);
    moveZAction = reinterpret_cast<ZInputAction*>(BaseAddress + 0xE537D8);
    tiltCameraAction = reinterpret_cast<ZInputAction*>(BaseAddress + 0xE53904);
    turnCameraAction = reinterpret_cast<ZInputAction*>(BaseAddress + 0xE537A4);
    leftTriggerAction = reinterpret_cast<ZInputAction*>(BaseAddress + 0xE538A4);
    rightTriggerAction = reinterpret_cast<ZInputAction*>(BaseAddress + 0xE537F0);
    rollModifierAction = reinterpret_cast<ZInputAction*>(BaseAddress + 0xE538E0);
    fovModifierAction = reinterpret_cast<ZInputAction*>(BaseAddress + 0xE53930);
    resetRollAction = reinterpret_cast<ZInputAction*>(BaseAddress + 0xE5384C);
    resetFovAction = reinterpret_cast<ZInputAction*>(BaseAddress + 0xE53744);
}

void FreeCamera::OnEngineInitialized()
{
    const ZMemberDelegate<FreeCamera, void(const SGameUpdateEvent&)> delegate(this, &FreeCamera::OnFrameUpdate);

    GameLoopManager->RegisterForFrameUpdate(delegate, 1);

    AddBindings();
}

void FreeCamera::OnDrawMenu()
{
    bool isFreeCameraActive = this->isFreeCameraActive;

    if (ImGui::Checkbox(ICON_MD_PHOTO_CAMERA " Free Camera", &isFreeCameraActive))
    {
        ToggleFreeCamera();
    }

    if (ImGui::Checkbox(ICON_MD_TIMER " Pause Game", &pauseGame))
    {
        GameTimeManager->SetPaused(pauseGame);
    }

    if (ImGui::Button(ICON_MD_SPORTS_ESPORTS " Free Camera Controls"))
    {
        areControlsVisible = !areControlsVisible;
    }
}

void FreeCamera::OnDrawUI(const bool hasFocus)
{
    if (areControlsVisible)
    {
        ImGui::PushFont(SDK::GetInstance().GetBoldFont());

        const auto areControlsExpanded = ImGui::Begin(ICON_MD_PHOTO_CAMERA " Free Camera Controls", &areControlsVisible);

        ImGui::PushFont(SDK::GetInstance().GetRegularFont());

        if (areControlsExpanded)
        {
            ImGui::TextUnformatted("PC Controls");
            ImGui::BeginTable("FreeCameraControlsPc", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_SizingFixedFit);

            for (auto& [key, description] : pcControls)
            {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(key.c_str());
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(description.c_str());
            }

            ImGui::EndTable();

            ImGui::TextUnformatted("Controller Controls");
            ImGui::BeginTable("FreeCameraControlsController", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_SizingFixedFit);

            for (auto& [key, description] : controllerControls)
            {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(key.c_str());
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(description.c_str());
            }

            ImGui::EndTable();
        }

        ImGui::PopFont();
        ImGui::End();
        ImGui::PopFont();
    }
}

void FreeCamera::OnFrameUpdate(const SGameUpdateEvent& updateEvent)
{
    if (HUDManager->IsPauseMenuActive())
    {
        return;
    }

    ZApplicationEngineWin32* applicationEngineWin32 = ZApplicationEngineWin32::GetInstance();

    if (!applicationEngineWin32)
    {
        return;
    }

    ZHitman5* hitman = LevelManager->GetHitman().GetRawPointer();

    if (!hitman)
    {
        return;
    }

    ZEngineAppCommon& engineAppCommon = applicationEngineWin32->GetEngineAppCommon();
    ZFreeCameraControlEntity* freeCameraControlEntity = engineAppCommon.GetFreeCameraControl().GetRawPointer();

    if (toggleFreeCameraAction.Digital())
    {
        ToggleFreeCamera();
    }

    if (toggleFreeCameraAndPauseGameAction.Digital())
    {
        ToggleFreeCamera();
        GameTimeManager->SetPaused(pauseGame);
    }

    if (isFreeCameraActive)
    {
        if (instantlyKillNpcAction.Digital())
        {
            InstantlyKillNpc();
        }

        if (teleportMainCharacterAction.Digital())
        {
            TeleportMainCharacter();
        }

        if (freezeFreeCameraAction.Digital())
        {
            freezeCamera = !freezeCamera;

            if (freeCameraControlEntity)
            {
                freeCameraControlEntity->SetActive(!freezeCamera);
            }

            ZHitman5* hitman = LevelManager->GetHitman().GetRawPointer();

            if (hitman)
            {
                ZHM5InputControl* inputControl = hitman->GetInputControl();

                if (inputControl)
                {
                    if (freezeCamera)
                    {
                        inputControl->EnableBindings();
                    }
                    else
                    {
                        inputControl->DisableBindings();
                    }
                }
            }
        }
    }

    if (pauseGameAction.Digital())
    {
        pauseGame = !pauseGame;

        GameTimeManager->SetPaused(pauseGame);
    }
}

void FreeCamera::ToggleFreeCamera()
{
    ZApplicationEngineWin32* applicationEngineWin32 = ZApplicationEngineWin32::GetInstance();
    ZEngineAppCommon& engineAppCommon = applicationEngineWin32->GetEngineAppCommon();

    if (!engineAppCommon.GetFreeCamera().GetRawPointer())
    {
        engineAppCommon.CreateFreeCameraAndControl();

        InputActionManager->AddBindings("FreeCamControl0={SpeedModifier0=| hold(gc0,b) | hold(gc0,circle) | hold(kb,lalt) hold(kb, ralt);};");
        InputActionManager->AddBindings("FreeCamControl1={SpeedModifier1=| hold(gc1,b) | hold(gc1,circle) | hold(kb,lalt) hold(kb, ralt);};");

        speedModifierAction[0] = ZInputAction("SpeedModifier0");
        speedModifierAction[1] = ZInputAction("SpeedModifier1");
    }

    isFreeCameraActive = !isFreeCameraActive;

    if (isFreeCameraActive)
    {
        EnableFreeCamera();
    }
    else
    {
        DisableFreeCamera();
    }

    engineAppCommon.GetFreeCameraControl().GetRawPointer()->SetActive(isFreeCameraActive);
}

void FreeCamera::EnableFreeCamera()
{
    ZApplicationEngineWin32* applicationEngineWin32 = ZApplicationEngineWin32::GetInstance();
    ZEngineAppCommon& engineAppCommon = applicationEngineWin32->GetEngineAppCommon();

    TEntityRef<ZCameraEntity> freeCamera = engineAppCommon.GetFreeCamera();

    if (!freeCamera.GetRawPointer())
    {
        return;
    }

    TEntityRef<IRenderDestinationEntity> renderDestinationEntity = RenderManager->GetGameRenderDestinationEntity();
    TEntityRef<ZCameraEntity> playerCamera = renderDestinationEntity.GetRawPointer()->GetSource();

    engineAppCommon.SetMainCamera(playerCamera);
    engineAppCommon.CopyMainCameraSettingsToFreeCamera();
    renderDestinationEntity.GetRawPointer()->SetSource(freeCamera.GetEntityRef());

    ZHitman5* hitman = LevelManager->GetHitman().GetRawPointer();

    if (hitman)
    {
        ZHM5InputControl* inputControl = hitman->GetInputControl();

        if (inputControl)
        {
            inputControl->DisableBindings();
        }
    }
}

void FreeCamera::DisableFreeCamera()
{
    ZApplicationEngineWin32* applicationEngineWin32 = ZApplicationEngineWin32::GetInstance();
    ZEngineAppCommon& engineAppCommon = applicationEngineWin32->GetEngineAppCommon();
    TEntityRef<IRenderDestinationEntity> renderDestinationEntity = RenderManager->GetGameRenderDestinationEntity();
    TEntityRef<ZCameraEntity> mainCamera;

    renderDestinationEntity.GetRawPointer()->SetSource(engineAppCommon.GetMainCamera().GetEntityRef());
    engineAppCommon.SetMainCamera(mainCamera);

    ZHitman5* hitman = LevelManager->GetHitman().GetRawPointer();

    if (hitman)
    {
        ZHM5InputControl* inputControl = hitman->GetInputControl();

        if (inputControl)
        {
            inputControl->EnableBindings();
        }
    }
}

void FreeCamera::OnCreateScene(ZEntitySceneContext* entitySceneContext, const ZString& streamingState)
{
    if (isFreeCameraActive)
    {
        DisableFreeCamera();
    }

    isFreeCameraActive = false;
}

void FreeCamera::OnClearScene(ZEntitySceneContext* entitySceneContext, bool fullyUnloadScene)
{
    if (isFreeCameraActive)
    {
        DisableFreeCamera();
    }

    isFreeCameraActive = false;
}

void FreeCamera::OnUpdateCamera(float delta)
{
    if (HUDManager->IsPauseMenuActive())
    {
        return;
    }

    ZApplicationEngineWin32* applicationEngineWin32 = ZApplicationEngineWin32::GetInstance();
    ZEngineAppCommon& engineAppCommon = applicationEngineWin32->GetEngineAppCommon();
    ZFreeCameraControlEntity* freeCameraControlEntity = engineAppCommon.GetFreeCameraControl().GetRawPointer();

    if (!freeCameraControlEntity)
    {
        return;
    }

    ZCameraEntity* controlledCameraEntity = freeCameraControlEntity->GetControlledCameraEntity();

    if (controlledCameraEntity)
    {
        UpdateFov(delta);

        persistentTranslationSpeedMultiplier = MoveValueWithinRange(
            (delta * deltaTranslationSpeed) * freeCamTranslationSpeedChangeSensitivity,
            persistentTranslationSpeedMultiplier,
            1.0,
            freeCamTranslationSpeedMin,
            freeCamTranslationSpeedMax);
        persistentRotationSpeedMultiplier = MoveValueWithinRange(
            (delta * deltaRotationSpeed) * freeCamRotationSpeedChangeSensitivity,
            persistentRotationSpeedMultiplier,
            1.0,
            freeCamRotationSpeedMin,
            freeCamRotationSpeedMax);

        float translationSpeed = ((temporaryTranslationSpeedMultiplier * freeCameraControlEntity->GetMoveSpeed()) * persistentTranslationSpeedMultiplier) * fovDependentSpeedMultiplier;
        float rotationSpeed = ((temporaryRotationSpeedMultiplier * freeCameraControlEntity->GetTurnSpeed()) * persistentRotationSpeedMultiplier) * fovDependentSpeedMultiplier;

        float moveX = (translationSpeed * freeCameraControlEntity->GetMoveX()) * delta;
        float moveY = (translationSpeed * freeCameraControlEntity->GetMoveY()) * delta;
        float moveZ = (translationSpeed * freeCameraControlEntity->GetMoveZ()) * delta;
        float deltaRoll = (rotationSpeed * freeCameraControlEntity->GetDeltaRoll()) * delta;
        float deltaYaw = (rotationSpeed * freeCameraControlEntity->GetDeltaYaw()) * delta;
        float deltaPitch = (rotationSpeed * freeCameraControlEntity->GetDeltaPitch()) * delta;

        bool updateCameraRotation = std::abs(deltaRoll) > 0.00024414062 ||
            std::abs(deltaYaw) > 0.00024414062 ||
            std::abs(deltaPitch) > 0.00024414062 ||
            freeCameraControlEntity->GetResetRoll();
        bool updateCameraPosition = std::abs(moveX) > 0.00024414062 ||
            std::abs(moveY) > 0.00024414062 ||
            std::abs(moveZ) > 0.00024414062;
        SMatrix currentCameraToWorld = controlledCameraEntity->GetObjectToWorldMatrix();

        if (updateCameraRotation)
        {
            currentCameraToWorld = freeCameraControlEntity->GetUpdatedCameraRotation(deltaRoll, deltaPitch, deltaYaw, currentCameraToWorld);
        }

        if (updateCameraPosition)
        {
            float4 updatedCameraPosition = freeCameraControlEntity->GetUpdatedCameraPosition(moveX, moveY, moveZ, currentCameraToWorld);

            currentCameraToWorld.mat[3] = updatedCameraPosition;
        }

        if (updateCameraRotation || updateCameraPosition)
        {
            controlledCameraEntity->SetObjectToWorldMatrix(currentCameraToWorld);
        }
    }
}

void FreeCamera::OnUpdateMovementFromInput()
{
    if (HUDManager->IsPauseMenuActive())
    {
        return;
    }

    ZApplicationEngineWin32* applicationEngineWin32 = ZApplicationEngineWin32::GetInstance();
    ZEngineAppCommon& engineAppCommon = applicationEngineWin32->GetEngineAppCommon();
    ZFreeCameraControlEntity* freeCameraControlEntity = engineAppCommon.GetFreeCameraControl().GetRawPointer();

    if (!freeCameraControlEntity)
    {
        return;
    }

    const int controllerID = freeCameraControlEntity->GetControllerID();

    freeCameraControlEntity->SetMoveX(0);
    freeCameraControlEntity->SetMoveY(0);
    freeCameraControlEntity->SetMoveZ(0);
    freeCameraControlEntity->SetDeltaPitch(0);
    freeCameraControlEntity->SetDeltaRoll(0);
    freeCameraControlEntity->SetDeltaFov(0);
    freeCameraControlEntity->SetMoveInWorldSpace(false);
    freeCameraControlEntity->SetResetRoll(false);

    deltaTranslationSpeed = 0.f;
    deltaRotationSpeed = 0.f;

    if (leftBumperAction[controllerID].Digital())
    {
        freeCameraControlEntity->SetIsGameControlActive(true);

        return;
    }

    float moveX = moveXAction->Analog() + analogLeftXAction[controllerID].Analog();

    if (moveX >= 1.f)
    {
        moveX = 1.f;
    }
    else if (moveX < -1.f)
    {
        moveX = -1.f;
    }

    float moveY = analogLeftYAction[controllerID].Analog() - moveYAction->Analog();

    if (moveY >= 1.f)
    {
        moveY = 1.f;
    }
    else if (moveY < -1.f)
    {
        moveY = -1.f;
    }

    float moveZ = analogRightYAction[controllerID].Analog() - moveZAction->Analog();

    if (moveZ >= 1.f)
    {
        moveZ = 1.f;
    }
    else if (moveZ < -1.f)
    {
        moveZ = -1.f;
    }

    float deltaPitch = analogRightYAction[controllerID].Analog() - tiltCameraAction->Analog() * 0.14285715f;

    if (deltaPitch >= 4.f)
    {
        deltaPitch = 4.f;
    }
    else if (deltaPitch < -4.f)
    {
        deltaPitch = -4.f;
    }

    float deltaYaw = turnCameraAction->Analog() * 0.14285715f + analogRightXAction[controllerID].Analog();

    if (deltaYaw >= 4.f)
    {
        deltaYaw = 4.f;
    }
    else if (deltaYaw < -4.f)
    {
        deltaYaw = -4.f;
    }

    temporaryTranslationSpeedMultiplier = rightTriggerAction[controllerID].Analog() * (freeCamTranslationSpeedMax - 1.f) + 1.f;
    temporaryRotationSpeedMultiplier = (rightTriggerAction[controllerID].Analog() * (freeCamRotationSpeedMax - 1.f)) + 1.f;

    if (leftTriggerAction[controllerID].Analog() > 0.60000002)
    {
        freeCameraControlEntity->SetMoveX(moveX);
        freeCameraControlEntity->SetMoveY(moveY);
        freeCameraControlEntity->SetMoveZ(moveZ);
        freeCameraControlEntity->SetMoveInWorldSpace(true);
    }
    else if (rightBumperAction[controllerID].Analog())
    {
        freeCameraControlEntity->SetMoveX(moveX);
        freeCameraControlEntity->SetMoveZ(analogLeftYAction->Digital());
        freeCameraControlEntity->SetDeltaPitch(deltaPitch);
        freeCameraControlEntity->SetDeltaYaw(deltaYaw);
    }
    else if (rollModifierAction[controllerID].Digital() || fovModifierAction[controllerID].Digital() || speedModifierAction[controllerID].Digital())
    {
        if (rollModifierAction[controllerID].Digital())
        {
            if (resetRollAction[controllerID].Digital())
            {
                freeCameraControlEntity->SetRoll(0);
                freeCameraControlEntity->SetResetRoll(true);
            }
            else
            {
                freeCameraControlEntity->SetDeltaRoll(moveX);
            }
        }

        if (fovModifierAction[controllerID].Digital())
        {
            if (resetFovAction[controllerID].Digital())
            {
                freeCameraControlEntity->SetFov(freeCameraControlEntity->GetInitialFov());
            }
            else
            {
                freeCameraControlEntity->SetDeltaFov(moveY);
            }
        }

        if (speedModifierAction[controllerID].Digital())
        {
            if (resetSpeedAction[controllerID].Digital())
            {
                persistentTranslationSpeedMultiplier = 1.f;
                persistentRotationSpeedMultiplier = 1.f;
            }
            else
            {
                if (std::abs(moveY) > freeCamSpeedChangeThreshold)
                {
                    deltaTranslationSpeed = moveY;
                }

                if (std::abs(moveX) > freeCamSpeedChangeThreshold)
                {
                    deltaRotationSpeed = moveX;
                }
            }
        }
    }
    else
    {
        freeCameraControlEntity->SetMoveX(moveX);
        freeCameraControlEntity->SetMoveY(moveY);
        freeCameraControlEntity->SetDeltaPitch(deltaPitch);
        freeCameraControlEntity->SetDeltaYaw(deltaYaw);
    }
}

void FreeCamera::UpdateFov(float delta)
{
    ZApplicationEngineWin32* applicationEngineWin32 = ZApplicationEngineWin32::GetInstance();
    ZEngineAppCommon& engineAppCommon = applicationEngineWin32->GetEngineAppCommon();
    ZFreeCameraControlEntity* freeCameraControlEntity = engineAppCommon.GetFreeCameraControl().GetRawPointer();
    float fov = (((((temporaryTranslationSpeedMultiplier * freeCameraControlEntity->GetMoveSpeed())
        * persistentTranslationSpeedMultiplier)
        * fovDependentSpeedMultiplier)
        * freeCameraControlEntity->GetDeltaFov())
        * delta)
        + freeCameraControlEntity->GetFov();

    if (fov >= 170.0)
    {
        fov = 170.f;
    }
    else if (fov < 5.0)
    {
        fov = 5.f;
    }
    
    freeCameraControlEntity->SetFov(fov);

    if (fov < freeCameraControlEntity->GetInitialFov())
    {
        fovDependentSpeedMultiplier = ZMath::MapRange01(fov, 5.f, freeCameraControlEntity->GetInitialFov()) * (1.f - freeCamFovDependentSpeedMin) + freeCamFovDependentSpeedMin;
    }
    else
    {
        fovDependentSpeedMultiplier = ZMath::MapRange01(fov, freeCameraControlEntity->GetInitialFov(), 170.f) * (freeCamFovDependentSpeedMax - 1.f) + 1.f;
    }

    ZCameraEntity* controlledCameraEntity = freeCameraControlEntity->GetControlledCameraEntity();

    if (controlledCameraEntity)
    {
        controlledCameraEntity->SetFovYDeg(fov);
    }
}

float FreeCamera::MoveValueWithinRange(float delta, float currentValue, float pivotValue, float rangeMin, float rangeMax)
{
    float result;

    if (currentValue < pivotValue)
    {
        result = (std::abs(rangeMin - 1.f) * 0.1f) * delta + currentValue;
    }
    else
    {
        result = (std::abs(rangeMax - 1.f) * 0.1f) * delta + currentValue;
    }

    if (result < rangeMin)
    {
        result = rangeMin;
    }
    else if (result > rangeMax)
    {
        result = rangeMax;
    }

    return result;
}

void FreeCamera::InstantlyKillNpc()
{
    ZRayQueryOutput rayQueryOutput{};

    if (GetFreeCameraRayCastClosestHitQueryOutput(ERayDetailLevel::RAYDETAILS_MESH, rayQueryOutput) && rayQueryOutput.GetBlockingEntity().GetEntityTypePtrPtr())
    {
        ZActor* actor = rayQueryOutput.GetBlockingEntity().QueryInterfacePtr<ZActor>();

        if (actor)
        {
            actor->KillActor(EActorDeathType::eADT_UNDEFINED, true);
        }
    }
}

void FreeCamera::TeleportMainCharacter()
{
    ZRayQueryOutput rayQueryOutput{};

    if (GetFreeCameraRayCastClosestHitQueryOutput(ERayDetailLevel::RAYDETAILS_BONES, rayQueryOutput) && rayQueryOutput.GetBlockingEntity().GetEntityTypePtrPtr())
    {
        const TEntityRef<ZHitman5>& hitman = LevelManager->GetHitman();

        if (hitman.GetRawPointer())
        {
            ZSpatialEntity* spatialEntity = hitman.GetEntityRef().QueryInterfacePtr<ZSpatialEntity>();
            SMatrix worldMatrix = spatialEntity->GetObjectToWorldMatrix();

            worldMatrix.Trans = rayQueryOutput.GetPosition();

            spatialEntity->SetObjectToWorldMatrix(worldMatrix);
        }
    }
}

bool FreeCamera::GetFreeCameraRayCastClosestHitQueryOutput(const ERayDetailLevel detailLevel, ZRayQueryOutput& rayQueryOutput)
{
    ZApplicationEngineWin32* applicationEngineWin32 = ZApplicationEngineWin32::GetInstance();
    ZEngineAppCommon& engineAppCommon = applicationEngineWin32->GetEngineAppCommon();
    TEntityRef<ZCameraEntity> freeCamera = engineAppCommon.GetFreeCamera();

    SMatrix worldMatrix = freeCamera.GetRawPointer()->GetObjectToWorldMatrix();
    float4 invertedDirection = float4(-worldMatrix.ZAxis.x, -worldMatrix.ZAxis.y, -worldMatrix.ZAxis.z, -worldMatrix.ZAxis.w);
    float4 from = worldMatrix.Trans;
    float4 to = worldMatrix.Trans + invertedDirection * 500.f;

    if (!CollisionManager)
    {
        return false;
    }

    ZRayQueryInput rayQueryInput = ZRayQueryInput(from, to, detailLevel);

    if (!CollisionManager->RayCastClosestHit(rayQueryInput, &rayQueryOutput))
    {
        return false;
    }

    return true;
}

void __fastcall ZEntitySceneContext_CreateSceneHook(ZEntitySceneContext* pThis, int edx, const ZString& sStreamingState)
{
    GetModInstance()->OnCreateScene(pThis, sStreamingState);

    Hooks::ZEntitySceneContext_CreateScene.CallOriginalFunction(pThis, sStreamingState);
}

void __fastcall ZEntitySceneContext_ClearSceneHook(ZEntitySceneContext* pThis, int edx, bool bFullyUnloadScene)
{
    GetModInstance()->OnClearScene(pThis, bFullyUnloadScene);

    Hooks::ZEntitySceneContext_ClearScene.CallOriginalFunction(pThis, bFullyUnloadScene);
}

void __fastcall ZFreeCameraControlEntity_UpdateCameraHook(ZFreeCameraControlEntity* pThis, int edx, float dt)
{
    GetModInstance()->OnUpdateCamera(dt);

    //Hooks::ZFreeCameraControlEntity_UpdateCamera.CallOriginalFunction(pThis, dt);
}

void __fastcall ZFreeCameraControlEntity_UpdateMovementFromInputHook(ZFreeCameraControlEntity* pThis, int edx)
{
    GetModInstance()->OnUpdateMovementFromInput();

    //Hooks::ZFreeCameraControlEntity_UpdateMovementFromInput.CallOriginalFunction(pThis);
}

void __fastcall ZEngineAppCommon_ResetSceneCallbackHook(ZEngineAppCommon* pThis, int edx)
{
    Hooks::ZEngineAppCommon_ResetSceneCallback.CallOriginalFunction(pThis);

    ZApplicationEngineWin32* applicationEngineWin32 = ZApplicationEngineWin32::GetInstance();
    TEntityRef<ZCameraEntity> freeCamera;

    applicationEngineWin32->GetEngineAppCommon().SetFreeCamera(freeCamera);
}

DEFINE_MOD(FreeCamera);
