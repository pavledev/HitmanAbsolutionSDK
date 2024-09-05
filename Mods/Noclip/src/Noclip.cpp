#include <IconsMaterialDesign.h>

#include <Glacier/ZGameLoopManager.h>
#include <Glacier/Input/ZInputActionManager.h>
#include <Glacier/ZLevelManager.h>

#include <Noclip.h>
#include <Hooks.h>

Noclip::Noclip() :
    isNoclipEnabled(false),
    toggleNoclipAction("ToggleNoclip")
{
}

Noclip::~Noclip()
{
    const ZMemberDelegate<Noclip, void(const SGameUpdateEvent&)> delegate(this, &Noclip::OnFrameUpdate);

    GameLoopManager->UnregisterForFrameUpdate(delegate);

    Hooks::ZEntitySceneContext_ClearScene.DisableHook();
    Hooks::ZEntitySceneContext_ClearScene.RemoveHook();
}

void Noclip::Initialize()
{
    ModInterface::Initialize();

    Hooks::ZEntitySceneContext_ClearScene.CreateHook("ZEntitySceneContext::ClearScene", 0x265A80, ZEntitySceneContext_ClearSceneHook);

    Hooks::ZEntitySceneContext_ClearScene.EnableHook();
}

void Noclip::OnEngineInitialized()
{
    const ZMemberDelegate<Noclip, void(const SGameUpdateEvent&)> delegate(this, &Noclip::OnFrameUpdate);

    GameLoopManager->RegisterForFrameUpdate(delegate, 1);

    const char* bindings = "NoclipInput={"
        "ToggleNoclip=tap(kb,n);};";

    InputActionManager->AddBindings(bindings);
}

void Noclip::OnDrawMenu()
{
    if (ImGui::Checkbox(ICON_MD_SELF_IMPROVEMENT " Items", &isNoclipEnabled))
    {
        if (isNoclipEnabled)
        {
            ZHitman5* hitman = LevelManager->GetHitman().GetRawPointer();

            if (hitman)
            {
                playerPosition = hitman->GetSpatialEntityPtr()->GetWorldPosition();
            }
        }
    }
}

void Noclip::OnFrameUpdate(const SGameUpdateEvent& updateEvent)
{
    ZHitman5* hitman = LevelManager->GetHitman().GetRawPointer();

    if (!hitman)
    {
        return;
    }

    if (toggleNoclipAction.Digital())
    {
        isNoclipEnabled = !isNoclipEnabled;

        if (isNoclipEnabled)
        {
            playerPosition = hitman->GetSpatialEntityPtr()->GetWorldPosition();
        }
    }

    if (!isNoclipEnabled)
    {
        return;
    }

    ZHM5MainCamera* mainCamera = hitman->GetMainCamera();

    if (!mainCamera)
    {
        return;
    }

    const SMatrix cameraTransform = mainCamera->GetObjectToWorldMatrix();
    float moveSpeed = 5.f;

    if (GetAsyncKeyState(VK_SHIFT))
        moveSpeed = 20.f;

    if (GetAsyncKeyState('W'))
        playerPosition += cameraTransform.Up * -moveSpeed * updateEvent.m_GameTimeDelta.ToSeconds();

    if (GetAsyncKeyState('S'))
        playerPosition += cameraTransform.Up * moveSpeed * updateEvent.m_GameTimeDelta.ToSeconds();

    if (GetAsyncKeyState('A'))
        playerPosition += cameraTransform.Left * -moveSpeed * updateEvent.m_GameTimeDelta.ToSeconds();

    if (GetAsyncKeyState('D'))
        playerPosition += cameraTransform.Left * moveSpeed * updateEvent.m_GameTimeDelta.ToSeconds();

    hitman->GetSpatialEntityPtr()->SetWorldPosition(playerPosition);
}

void Noclip::OnClearScene(ZEntitySceneContext* entitySceneContext, bool fullyUnloadScene)
{
    isNoclipEnabled = false;
}

void __fastcall ZEntitySceneContext_ClearSceneHook(ZEntitySceneContext* pThis, int edx, bool bFullyUnloadScene)
{
    GetModInstance()->OnClearScene(pThis, bFullyUnloadScene);

    Hooks::ZEntitySceneContext_ClearScene.CallOriginalFunction(pThis, bFullyUnloadScene);
}

DEFINE_MOD(Noclip);
