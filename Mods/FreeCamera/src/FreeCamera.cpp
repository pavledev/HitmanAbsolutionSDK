#include "FreeCamera.h"

#include "imgui.h"

#include "IconsMaterialDesign.h"

#include <Glacier/ZEntity.h>
#include <Glacier/ZRender.h>
#include <Glacier/ZLevelManager.h>
#include <Glacier/ZApplication.h>
#include <Glacier/ZInput.h>
#include <Glacier/ZMath.h>
#include <Glacier/ZPhysics.h>
#include <Glacier/ZActor.h>
#include <Glacier/ZScaleform.h>
#include <Glacier/ZGameTimeManager.h>

#include "SDK.h"
#include "Hooks.h"

FreeCamera::FreeCamera()
    : m_IsFreeCameraActive(false)
    , m_ShouldToggle(false)
    , m_IsGamePaused(false)
    , m_IsPlayerInputEnabled(false)
    , m_IsFreeCameraFrozen(false)
    , m_ToggleFreeCameraAction("ToggleFreeCamera")
    , m_ActivatePlayerInputAction("ActivatePlayerInput")
    , m_ActivateGameControlAction("ActivateGameControl")
    , m_TogglePauseGameAction("TogglePauseGame")
    , m_TeleportPlayerAction("TeleportPlayer")
    , m_KillActorAction("KillActor")
    , m_ShowFreeCameraWindow(false)
    , m_ShowControlsWindow(false)
    , m_FreeCamSpeedChangeThreshold(0.5f)
    , m_DeltaTranslationSpeed(0.f)
    , m_FreeCamTranslationSpeedChangeSensitivity(4.f)
    , m_FreeCamTranslationSpeedMin(0.1f)
    , m_FreeCamTranslationSpeedMax(10.f)
    , m_FreeCamRotationSpeedMin(0.1f)
    , m_FreeCamRotationSpeedMax(2.f)
    , m_FreeCamRotationSpeedChangeSensitivity(2.f)
    , m_FreeCamFovDependentSpeedMin(0.1f)
    , m_FreeCamFovDependentSpeedMax(2.f)
    , m_FOVDependentSpeedMultiplier(1.f)
    , m_PersistentTranslationSpeedMultiplier(0x3F800000)
    , m_PersistentRotationSpeedMultiplier(0x3F800000)
{
    m_PcControls = {
        {"K", "Toggle freecam"},
        {"F3", "Freeze camera and enable player input"},

        {"W / S", "Move camera forward/backward"},
        {"A / D", "Move camera left/right"},
        {"Q / E", "Move camera down/up"},
        {"Arrow Keys", "Move camera"},

        {"Mouse", "Rotate camera"},

        {"Ctrl + A/D", "Roll camera"},
        {"Ctrl + W/S", "Change FOV"},

        {"Alt + W/S", "Change camera speed"},
        {"Alt + A/D", "Change rotation speed"},

        {"Ctrl + X", "Reset roll"},
        {"Ctrl + Z", "Reset FOV"},
        {"Alt + Z", "Reset camera speed"},

        {"Space + W/A/S/D", "Move camera in world space"},
        {"Space + Q/E", "Move camera vertically in world space"},

        {"Shift", "Temporary speed boost"},
        {"F", "Fixed-degree camera rotation"},

        {"Ctrl + F6", "Teleport player"},
        {"F9", "Kill humanoid"},

        {"F8", "Pause/Resume game"}
    };

    m_ControllerControls = {
        {"Right Stick", "Rotate camera"},
        {"Left Stick", "Move camera"},

        {"RB", "Move camera vertically"},
        {"RT", "Temporary speed boost"},

        {"A + Left Stick", "Roll camera"},
        {"Y + Left Stick", "Change FOV"},
        {"B + Left Stick", "Change camera speed"},

        {"Left Stick Press", "Reset roll/FOV/speed"},

        {"LT + Left Stick", "Move camera in world space"},
        {"LT + Right Stick Vertical", "Move camera vertically in world space"},

        {"LB", "Freeze camera and enable player input"}
    };
}

FreeCamera::~FreeCamera()
{
    const ZMemberDelegate<FreeCamera, void(const SGameUpdateEvent&)> delegate(this, &FreeCamera::OnFrameUpdate);
    Globals::GameLoopManager->UnregisterForFrameUpdate(delegate);

    if (m_IsFreeCameraActive)
    {
        ZApplicationEngineWin32* applicationEngineWin32 = *Globals::ApplicationEngineWin32;
        TEntityRef<IRenderDestinationEntity> renderDestinationEntity = Globals::RenderManager->GetGameRenderDestinationEntity();

        renderDestinationEntity.m_pInterfaceRef->SetSource(applicationEngineWin32->m_common.m_pMainCamera.m_entityRef);

        if (!m_IsPlayerInputEnabled)
        {
            TEntityRef<ZHitman5> hitman = Globals::LevelManager->m_rHitman;

            if (hitman.m_pInterfaceRef)
            {
                ZHM5InputControl* inputControl = hitman.m_pInterfaceRef->m_pInputControl;

                if (inputControl)
                {
                    inputControl->EnableBindings();
                }
            }
        }
    }
}

void FreeCamera::Initialize()
{
    Hooks::ZEntitySceneContext_ClearScene->AddDetour(this, &FreeCamera::ZEntitySceneContext_ClearScene);
    Hooks::ZFreeCameraControlEntity_UpdateCamera->AddDetour(this, &FreeCamera::ZFreeCameraControlEntity_UpdateCamera);
    Hooks::ZFreeCameraControlEntity_UpdateMovementFromInput->AddDetour(this, &FreeCamera::ZFreeCameraControlEntity_UpdateMovementFromInput);
    Hooks::ZEngineAppCommon_ResetSceneCallback->AddDetour(this, &FreeCamera::ZEngineAppCommon_ResetSceneCallback);
}

void FreeCamera::OnEngineInitialized()
{
    const ZMemberDelegate<FreeCamera, void(const SGameUpdateEvent&)> delegate(this, &FreeCamera::OnFrameUpdate);
    Globals::GameLoopManager->RegisterForFrameUpdate(delegate, 1);

    const char* bindings = "FreeCameraInput={"
                           "ToggleFreeCamera=tap(kb,k);"
                           "TogglePauseGame=tap(kb,f8);"
                           "ActivatePlayerInput=tap(kb,f3);"
                           "TeleportPlayer=& hold(kb,lctrl) tap(kb,f6);"
                           "KillHumanoid=tap(kb,f9);};";

    Globals::InputActionManager->AddBindings(bindings);
}

void FreeCamera::OnDrawMenu()
{
    if (ImGui::Button(ICON_MD_PHOTO_CAMERA " Free camera"))
    {
        m_ShowFreeCameraWindow = !m_ShowFreeCameraWindow;
    }
}

void FreeCamera::OnDrawUI(const bool hasFocus)
{
    if (!hasFocus)
    {
        return;
    }

    if (m_ShowFreeCameraWindow)
    {
        const auto center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        ImGui::PushFont(SDK::GetInstance().GetBoldFont());
        const auto isWindowExpanded = ImGui::Begin(ICON_MD_PHOTO_CAMERA " FreeCam", &m_ShowFreeCameraWindow);
        ImGui::PushFont(SDK::GetInstance().GetRegularFont());

        if (isWindowExpanded)
        {
            bool isFreeCamActive = m_IsFreeCameraActive;

            if (ImGui::Checkbox("Enable free camera", &isFreeCamActive))
            {
                ToggleFreeCamera();
            }

            bool isPlayerInputEnabled = m_IsPlayerInputEnabled;

            if (ImGui::Checkbox("Enable player input", &isPlayerInputEnabled))
            {
                TogglePlayerInput();
            }

            if (ImGui::Checkbox("Pause game in freecam", &m_IsGamePaused))
            {
                Globals::GameTimeManager->m_bPaused = m_IsGamePaused;
            }

            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("Whether the player should move along with the camera when in freecam.");
            }

            if (ImGui::Button(ICON_MD_SPORTS_ESPORTS " Show freecam controls"))
            {
                m_ShowControlsWindow = !m_ShowControlsWindow;
            }
        }

        ImGui::PopFont();
        ImGui::End();
        ImGui::PopFont();
    }

    if (m_ShowControlsWindow)
    {
        ImGui::PushFont(SDK::GetInstance().GetBoldFont());

        const auto areControlsExpanded = ImGui::Begin(ICON_MD_PHOTO_CAMERA " Free Camera Controls", &m_ShowControlsWindow);

        ImGui::PushFont(SDK::GetInstance().GetRegularFont());

        if (areControlsExpanded)
        {
            ImGui::TextUnformatted("PC Controls");

            if (ImGui::BeginTable("FreeCamControlsPc", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_SizingFixedFit))
            {
                for (auto& [key, description] : m_PcControls)
                {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted(key.c_str());
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted(description.c_str());
                }

                ImGui::EndTable();
            }

            ImGui::TextUnformatted("Controller Controls");

            if (ImGui::BeginTable("FreeCamControlsController", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_SizingFixedFit))
            {
                for (auto& [key, description] : m_ControllerControls)
                {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted(key.c_str());
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted(description.c_str());
                }

                ImGui::EndTable();
            }
        }

        ImGui::PopFont();
        ImGui::End();
        ImGui::PopFont();
    }
}

void FreeCamera::OnFrameUpdate(const SGameUpdateEvent& updateEvent)
{
    if (Globals::HUDManager->m_bPauseMenuActive)
    {
        return;
    }

    ZApplicationEngineWin32* applicationEngineWin32 = *Globals::ApplicationEngineWin32;

    if (!applicationEngineWin32)
    {
        return;
    }

    ZHitman5* hitman = Globals::LevelManager->m_rHitman.m_pInterfaceRef;

    if (!hitman)
    {
        return;
    }

    ZFreeCameraControlEntity* freeCameraControlEntity = applicationEngineWin32->m_common.m_pFreeCameraControl.m_pInterfaceRef;

    if (m_ToggleFreeCameraAction.Digital())
    {
        ToggleFreeCamera();
    }

    if (m_ShouldToggle)
    {
        m_ShouldToggle = false;

        if (m_IsFreeCameraActive)
        {
            EnableFreeCamera();
        }
        else
        {
            DisableFreeCamera();
        }
    }

    if (m_IsFreeCameraActive)
    {
        if (m_TogglePauseGameAction.Digital())
        {
            m_IsGamePaused = !m_IsGamePaused;
            Globals::GameTimeManager->m_bPaused = m_IsGamePaused;
        }

        if (m_ActivatePlayerInputAction.Digital() || m_ActivateGameControlAction.Digital())
        {
            TogglePlayerInput();
            SetFreeCamFrozen(m_IsPlayerInputEnabled);
        }

        if (m_TeleportPlayerAction.Digital())
        {
            TeleportPlayer();
        }

        if (m_KillActorAction.Digital())
        {
            KillActor();
        }
    }
}

void FreeCamera::ToggleFreeCamera()
{
    m_IsFreeCameraActive = !m_IsFreeCameraActive;
    m_ShouldToggle = true;
}

void FreeCamera::EnableFreeCamera()
{
    ZApplicationEngineWin32* applicationEngineWin32 = *Globals::ApplicationEngineWin32;

    if (!applicationEngineWin32->m_common.m_pFreeCamera)
    {
        applicationEngineWin32->m_common.CreateFreeCameraAndControl();

        Globals::InputActionManager->AddBindings(
            "FreeCamControl0={"
            "SpeedModifier0=| hold(gc0,b) | hold(gc0,circle) | hold(kb,lalt) hold(kb, ralt);"
            "ResetSpeed0=| hold(gc0, leftstick) | hold(gc0, left_thumb) hold(kb, z);"
            "};"
        );
        Globals::InputActionManager->AddBindings(
            "FreeCamControl1={"
            "SpeedModifier1=| hold(gc1,b) | hold(gc1,circle) | hold(kb,lalt) hold(kb, ralt);"
            "ResetSpeed1=| hold(gc1, leftstick) | hold(gc1, left_thumb) hold(kb, z);"
            "};"
        );
    }

    if (!applicationEngineWin32->m_common.m_pFreeCamera)
    {
        return;
    }

    TEntityRef<IRenderDestinationEntity> renderDestinationEntity = Globals::RenderManager->GetGameRenderDestinationEntity();
    TEntityRef<ZCameraEntity> playerCamera = renderDestinationEntity.m_pInterfaceRef->GetSource();

    applicationEngineWin32->m_common.m_pMainCamera = playerCamera;
    applicationEngineWin32->m_common.CopyMainCameraSettingsToFreeCamera();
    renderDestinationEntity.m_pInterfaceRef->SetSource(applicationEngineWin32->m_common.m_pFreeCamera.m_entityRef);

    if (!m_IsPlayerInputEnabled)
    {
        ZHitman5* hitman = Globals::LevelManager->m_rHitman.m_pInterfaceRef;

        if (hitman)
        {
            ZHM5InputControl* inputControl = hitman->m_pInputControl;

            if (inputControl)
            {
                inputControl->DisableBindings();
            }
        }
    }

    if (m_IsGamePaused)
    {
        Globals::GameTimeManager->m_bPaused = true;
    }

    applicationEngineWin32->m_common.m_pFreeCameraControl.m_pInterfaceRef->SetActive(true);
}

void FreeCamera::DisableFreeCamera()
{
    ZApplicationEngineWin32* applicationEngineWin32 = *Globals::ApplicationEngineWin32;
    TEntityRef<IRenderDestinationEntity> renderDestinationEntity = Globals::RenderManager->GetGameRenderDestinationEntity();
    TEntityRef<ZCameraEntity> mainCamera;

    renderDestinationEntity.m_pInterfaceRef->SetSource(applicationEngineWin32->m_common.m_pMainCamera.m_entityRef);
    applicationEngineWin32->m_common.m_pMainCamera = mainCamera;

    if (!m_IsPlayerInputEnabled)
    {
        ZHitman5* hitman = Globals::LevelManager->m_rHitman.m_pInterfaceRef;

        if (hitman)
        {
            ZHM5InputControl* inputControl = hitman->m_pInputControl;

            if (inputControl)
            {
                inputControl->EnableBindings();
            }
        }
    }

    Globals::GameTimeManager->m_bPaused = false;

    applicationEngineWin32->m_common.m_pFreeCameraControl.m_pInterfaceRef->SetActive(false);
}

void FreeCamera::TogglePlayerInput()
{
    m_IsPlayerInputEnabled = !m_IsPlayerInputEnabled;

    ZHitman5* hitman = Globals::LevelManager->m_rHitman.m_pInterfaceRef;

    if (!hitman)
    {
        return;
    }

    ZHM5InputControl* inputControl = hitman->m_pInputControl;

    if (!inputControl)
    {
        return;
    }

    if (m_IsPlayerInputEnabled)
    {
        inputControl->EnableBindings();
    }
    else
    {
        inputControl->DisableBindings();
    }
}

void FreeCamera::SetFreeCamFrozen(bool p_Frozen)
{
    m_IsFreeCameraFrozen = p_Frozen;

    auto freeCameraControl = (*Globals::ApplicationEngineWin32)->m_common.m_pFreeCameraControl;

    if (freeCameraControl.m_pInterfaceRef)
    {
        freeCameraControl.m_pInterfaceRef->m_bActive = !p_Frozen;
    }
}

void FreeCamera::UpdateFov(float p_Delta)
{
    const auto freeCameraControlEntity = (*Globals::ApplicationEngineWin32)->m_common.m_pFreeCameraControl.m_pInterfaceRef;

    float fov = (((((m_TemporaryTranslationSpeedMultiplier * freeCameraControlEntity->m_fMoveSpeed) * m_PersistentTranslationSpeedMultiplier)
                   * m_FOVDependentSpeedMultiplier)
                  * freeCameraControlEntity->m_fDeltaFov)
                 * p_Delta)
                + freeCameraControlEntity->m_fFov;

    if (fov >= 170.f)
    {
        fov = 170.f;
    }
    else if (fov < 5.0)
    {
        fov = 5.f;
    }

    freeCameraControlEntity->m_fFov = fov;

    if (fov < freeCameraControlEntity->m_fInitialFov)
    {
        m_FOVDependentSpeedMultiplier = ZMath::MapRange01(fov, 5.f, freeCameraControlEntity->m_fInitialFov) * (1.f - m_FreeCamFovDependentSpeedMin)
                                        + m_FreeCamFovDependentSpeedMin;
    }
    else
    {
        m_FOVDependentSpeedMultiplier =
            ZMath::MapRange01(fov, freeCameraControlEntity->m_fInitialFov, 170.f) * (m_FreeCamFovDependentSpeedMax - 1.f) + 1.f;
    }

    if (freeCameraControlEntity->m_pControlledCameraEntity)
    {
        freeCameraControlEntity->m_pControlledCameraEntity->SetFovYDeg(fov);
    }
}

float FreeCamera::MoveValueWithinRange(
    const float p_Delta, const float p_CurrentValue, const float p_PivotValue, const float p_RangeMin, const float p_RangeMax
)
{
    float result;

    if (p_CurrentValue < p_PivotValue)
    {
        result = (std::abs(p_RangeMin - 1.f) * 0.1f) * p_Delta + p_CurrentValue;
    }
    else
    {
        result = (std::abs(p_RangeMax - 1.f) * 0.1f) * p_Delta + p_CurrentValue;
    }

    if (result < p_RangeMin)
    {
        result = p_RangeMin;
    }
    else if (result > p_RangeMax)
    {
        result = p_RangeMax;
    }

    return result;
}

bool FreeCamera::RaycastFromFreeCamera(ZRayQueryOutput& p_RayQueryOutput)
{
    ZApplicationEngineWin32* applicationEngineWin32 = *Globals::ApplicationEngineWin32;
    TEntityRef<ZCameraEntity> freeCamera = applicationEngineWin32->m_common.m_pFreeCamera;

    SMatrix worldMatrix = freeCamera.m_pInterfaceRef->GetObjectToWorldMatrix();
    float4 invertedDirection = float4(-worldMatrix.ZAxis.x, -worldMatrix.ZAxis.y, -worldMatrix.ZAxis.z, -worldMatrix.ZAxis.w);
    float4 from = worldMatrix.Trans;
    float4 to = worldMatrix.Trans + invertedDirection * 500.f;

    if (!Globals::CollisionManager)
    {
        return false;
    }

    ZRayQueryInput rayQueryInput = ZRayQueryInput(from, to, ERayDetailLevel::RAYDETAILS_MESH);

    if (!Globals::CollisionManager->RayCastClosestHit(rayQueryInput, &p_RayQueryOutput))
    {
        return false;
    }

    return true;
}

void FreeCamera::TeleportPlayer()
{
    ZRayQueryOutput rayQueryOutput{};

    if (RaycastFromFreeCamera(rayQueryOutput) && rayQueryOutput.m_BlockingEntity.m_pEntityTypePtrPtr)
    {
        if (Globals::LevelManager->m_rHitman.m_pInterfaceRef)
        {
            ZSpatialEntity* spatialEntity = Globals::LevelManager->m_rHitman.m_entityRef.QueryInterfacePtr<ZSpatialEntity>();
            SMatrix worldMatrix = spatialEntity->GetObjectToWorldMatrix();

            worldMatrix.Trans = rayQueryOutput.m_vPosition;

            spatialEntity->SetObjectToWorldMatrix(worldMatrix);
        }
    }
}

void FreeCamera::KillActor()
{
    ZRayQueryOutput rayQueryOutput{};

    if (RaycastFromFreeCamera(rayQueryOutput) && rayQueryOutput.m_BlockingEntity.m_pEntityTypePtrPtr)
    {
        ZActor* actor = rayQueryOutput.m_BlockingEntity.QueryInterfacePtr<ZActor>();

        if (actor)
        {
            actor->KillActor(EActorDeathType::eADT_UNDEFINED, true);
        }
    }
}

DEFINE_THISCALL_DETOUR_WITH_CONTEXT(
    FreeCamera, void, ZEntitySceneContext_ClearScene, ZEntitySceneContext* p_EntitySceneContext, bool p_FullyUnloadScene
)
{
    if (m_IsFreeCameraActive)
    {
        DisableFreeCamera();
    }

    m_IsFreeCameraActive = false;
    m_ShouldToggle = false;

    return {HookAction::Continue()};
}

DEFINE_THISCALL_DETOUR_WITH_CONTEXT(FreeCamera, void, ZEngineAppCommon_ResetSceneCallback, ZEngineAppCommon* p_EngineAppCommon)
{
    p_Hook->CallOriginal(p_EngineAppCommon);

    ZApplicationEngineWin32* applicationEngineWin32 = *Globals::ApplicationEngineWin32;
    TEntityRef<ZCameraEntity> freeCamera;

    applicationEngineWin32->m_common.m_pFreeCamera = freeCamera;

    return {HookAction::Return()};
}

DEFINE_THISCALL_DETOUR_WITH_CONTEXT(
    FreeCamera, void, ZFreeCameraControlEntity_UpdateCamera, ZFreeCameraControlEntity* p_FreeCameraControlEntity, float p_Dt
)
{
    if (!m_IsFreeCameraActive)
    {
        return {HookAction::Return()};
    }

    if (Globals::HUDManager->m_bPauseMenuActive)
    {
        return {HookAction::Return()};
    }

    ZApplicationEngineWin32* applicationEngineWin32 = *Globals::ApplicationEngineWin32;
    ZFreeCameraControlEntity* freeCameraControlEntity = applicationEngineWin32->m_common.m_pFreeCameraControl.m_pInterfaceRef;

    if (!freeCameraControlEntity)
    {
        return {HookAction::Return()};
    }

    if (freeCameraControlEntity->m_pControlledCameraEntity)
    {
        UpdateFov(p_Dt);

        m_PersistentTranslationSpeedMultiplier = MoveValueWithinRange(
            (p_Dt * m_DeltaTranslationSpeed) * m_FreeCamTranslationSpeedChangeSensitivity, m_PersistentTranslationSpeedMultiplier, 1.0,
            m_FreeCamTranslationSpeedMin, m_FreeCamTranslationSpeedMax
        );
        m_PersistentRotationSpeedMultiplier = MoveValueWithinRange(
            (p_Dt * m_DeltaRotationSpeed) * m_FreeCamRotationSpeedChangeSensitivity, m_PersistentRotationSpeedMultiplier, 1.0,
            m_FreeCamRotationSpeedMin, m_FreeCamRotationSpeedMax
        );

        float translationSpeed =
            ((m_TemporaryTranslationSpeedMultiplier * freeCameraControlEntity->m_fMoveSpeed) * m_PersistentTranslationSpeedMultiplier)
            * m_FOVDependentSpeedMultiplier;
        float rotationSpeed = ((m_TemporaryRotationSpeedMultiplier * freeCameraControlEntity->m_fTurnSpeed) * m_PersistentRotationSpeedMultiplier)
                              * m_FOVDependentSpeedMultiplier;

        float moveX = (translationSpeed * freeCameraControlEntity->m_fMoveX) * p_Dt;
        float moveY = (translationSpeed * freeCameraControlEntity->m_fMoveY) * p_Dt;
        float moveZ = (translationSpeed * freeCameraControlEntity->m_fMoveZ) * p_Dt;
        float deltaRoll = (rotationSpeed * freeCameraControlEntity->m_fDeltaRoll) * p_Dt;
        float deltaYaw = (rotationSpeed * freeCameraControlEntity->m_fDeltaYaw) * p_Dt;
        float deltaPitch = (rotationSpeed * freeCameraControlEntity->m_fDeltaPitch) * p_Dt;

        bool updateCameraRotation = std::abs(deltaRoll) > 0.00024414062 || std::abs(deltaYaw) > 0.00024414062 || std::abs(deltaPitch) > 0.00024414062
                                    || freeCameraControlEntity->m_bResetRoll;
        bool updateCameraPosition = std::abs(moveX) > 0.00024414062 || std::abs(moveY) > 0.00024414062 || std::abs(moveZ) > 0.00024414062;
        SMatrix currentCameraToWorld = freeCameraControlEntity->m_pControlledCameraEntity->GetObjectToWorldMatrix();

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
            freeCameraControlEntity->m_pControlledCameraEntity->SetObjectToWorldMatrix(currentCameraToWorld);
        }
    }

    return {HookAction::Return()};
}

DEFINE_THISCALL_DETOUR_WITH_CONTEXT(
    FreeCamera, void, ZFreeCameraControlEntity_UpdateMovementFromInput, ZFreeCameraControlEntity* p_FreeCameraControlEntity
)
{
    if (!m_IsFreeCameraActive)
    {
        return {HookAction::Return()};
    }

    if (Globals::HUDManager->m_bPauseMenuActive)
    {
        return {HookAction::Return()};
    }

    ZApplicationEngineWin32* applicationEngineWin32 = *Globals::ApplicationEngineWin32;
    ZFreeCameraControlEntity* freeCameraControlEntity = applicationEngineWin32->m_common.m_pFreeCameraControl.m_pInterfaceRef;

    if (!freeCameraControlEntity)
    {
        return {HookAction::Return()};
    }

    freeCameraControlEntity->m_fMoveX = 0;
    freeCameraControlEntity->m_fMoveY = 0;
    freeCameraControlEntity->m_fMoveZ = 0;
    freeCameraControlEntity->m_fDeltaPitch = 0;
    freeCameraControlEntity->m_fDeltaRoll = 0;
    freeCameraControlEntity->m_fDeltaFov = 0;
    freeCameraControlEntity->m_bMoveInWorldSpace = false;
    freeCameraControlEntity->m_bResetRoll = false;

    m_DeltaTranslationSpeed = 0.f;
    m_DeltaRotationSpeed = 0.f;

    if (m_LeftBumperAction[freeCameraControlEntity->m_nControllerId].Digital())
    {
        freeCameraControlEntity->m_bIsGameControlActive = true;

        return {HookAction::Return()};
    }

    float moveX = m_MoveXAction.Analog() + m_AnalogLeftXAction[freeCameraControlEntity->m_nControllerId].Analog();

    if (moveX >= 1.f)
    {
        moveX = 1.f;
    }
    else if (moveX < -1.f)
    {
        moveX = -1.f;
    }

    float moveY = m_AnalogLeftYAction[freeCameraControlEntity->m_nControllerId].Analog() - m_MoveYAction.Analog();

    if (moveY >= 1.f)
    {
        moveY = 1.f;
    }
    else if (moveY < -1.f)
    {
        moveY = -1.f;
    }

    float moveZ = m_AnalogRightYAction[freeCameraControlEntity->m_nControllerId].Analog() - m_MoveZAction.Analog();

    if (moveZ >= 1.f)
    {
        moveZ = 1.f;
    }
    else if (moveZ < -1.f)
    {
        moveZ = -1.f;
    }

    float deltaPitch = m_AnalogRightYAction[freeCameraControlEntity->m_nControllerId].Analog() - m_TiltCameraAction.Analog() * 0.14285715f;

    if (deltaPitch >= 4.f)
    {
        deltaPitch = 4.f;
    }
    else if (deltaPitch < -4.f)
    {
        deltaPitch = -4.f;
    }

    float deltaYaw = m_TurnCameraAction.Analog() * 0.14285715f + m_AnalogRightXAction[freeCameraControlEntity->m_nControllerId].Analog();

    if (deltaYaw >= 4.f)
    {
        deltaYaw = 4.f;
    }
    else if (deltaYaw < -4.f)
    {
        deltaYaw = -4.f;
    }

    m_TemporaryTranslationSpeedMultiplier =
        m_RightTriggerAction[freeCameraControlEntity->m_nControllerId].Analog() * (m_FreeCamTranslationSpeedMax - 1.f) + 1.f;
    m_TemporaryRotationSpeedMultiplier =
        (m_RightTriggerAction[freeCameraControlEntity->m_nControllerId].Analog() * (m_FreeCamRotationSpeedMax - 1.f)) + 1.f;

    if (m_LeftTriggerAction[freeCameraControlEntity->m_nControllerId].Analog() > 0.60000002)
    {
        freeCameraControlEntity->m_fMoveX = moveX;
        freeCameraControlEntity->m_fMoveY = moveY;
        freeCameraControlEntity->m_fMoveZ = moveZ;
        freeCameraControlEntity->m_bMoveInWorldSpace = true;
    }
    else if (m_RightBumperAction[freeCameraControlEntity->m_nControllerId].Analog())
    {
        freeCameraControlEntity->m_fMoveX = moveX;
        freeCameraControlEntity->m_fMoveZ = m_AnalogLeftYAction[freeCameraControlEntity->m_nControllerId].Digital();
        freeCameraControlEntity->m_fDeltaPitch = deltaPitch;
        freeCameraControlEntity->m_fDeltaYaw = deltaYaw;
    }
    else if (
        m_RollModifierAction[freeCameraControlEntity->m_nControllerId].Digital()
        || m_FOVModifierAction[freeCameraControlEntity->m_nControllerId].Digital()
        || m_SpeedModifierAction[freeCameraControlEntity->m_nControllerId].Digital()
    )
    {
        if (m_RollModifierAction[freeCameraControlEntity->m_nControllerId].Digital())
        {
            if (m_ResetRollAction[freeCameraControlEntity->m_nControllerId].Digital())
            {
                freeCameraControlEntity->m_fRoll = 0;
                freeCameraControlEntity->m_bResetRoll = true;
            }
            else
            {
                freeCameraControlEntity->m_fDeltaRoll = moveX;
            }
        }

        if (m_FOVModifierAction[freeCameraControlEntity->m_nControllerId].Digital())
        {
            if (m_ResetFovAction[freeCameraControlEntity->m_nControllerId].Digital())
            {
                freeCameraControlEntity->m_fFov = freeCameraControlEntity->m_fInitialFov;
            }
            else
            {
                freeCameraControlEntity->m_fDeltaFov = moveY;
            }
        }

        if (m_SpeedModifierAction[freeCameraControlEntity->m_nControllerId].Digital())
        {
            if (m_ResetSpeedAction[freeCameraControlEntity->m_nControllerId].Digital())
            {
                m_PersistentTranslationSpeedMultiplier = 1.f;
                m_PersistentRotationSpeedMultiplier = 1.f;
            }
            else
            {
                if (std::abs(moveY) > m_FreeCamSpeedChangeThreshold)
                {
                    m_DeltaTranslationSpeed = moveY;
                }

                if (std::abs(moveX) > m_FreeCamSpeedChangeThreshold)
                {
                    m_DeltaRotationSpeed = moveX;
                }
            }
        }
    }
    else
    {
        freeCameraControlEntity->m_fMoveX = moveX;
        freeCameraControlEntity->m_fMoveY = moveY;
        freeCameraControlEntity->m_fDeltaPitch = deltaPitch;
        freeCameraControlEntity->m_fDeltaYaw = deltaYaw;
    }

    return {HookAction::Return()};
}

DEFINE_MOD(FreeCamera);
