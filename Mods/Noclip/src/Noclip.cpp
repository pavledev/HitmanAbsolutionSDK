#include "Noclip.h"

#include <IconsMaterialDesign.h>

#include <Glacier/ZGameLoopManager.h>
#include <Glacier/ZLevelManager.h>

#include <Hooks.h>

Noclip::Noclip()
    : m_IsNoclipEnabled(false)
    , m_ToggleNoclipAction("ToggleNoclip")
    , m_ForwardAction("Forward")
    , m_BackwardAction("Backward")
    , m_LeftAction("Left")
    , m_RightAction("Right")
    , m_FastAction("Fast")
{}

Noclip::~Noclip()
{
    const ZMemberDelegate<Noclip, void(const SGameUpdateEvent&)> delegate(this, &Noclip::OnFrameUpdate);
    Globals::GameLoopManager->UnregisterForFrameUpdate(delegate);
}

void Noclip::Initialize()
{
    Hooks::ZEntitySceneContext_ClearScene->AddDetour(this, &Noclip::ZEntitySceneContext_ClearScene);
}

void Noclip::OnEngineInitialized()
{
    const ZMemberDelegate<Noclip, void(const SGameUpdateEvent&)> delegate(this, &Noclip::OnFrameUpdate);
    Globals::GameLoopManager->RegisterForFrameUpdate(delegate, 1);

    const char* bindings = "NoclipInput={"
                           "ToggleNoclip=& hold(kb,lctrl) tap(kb,n);"
                           "Forward=hold(kb,w);"
                           "Backward=hold(kb,s);"
                           "Left=hold(kb,a);"
                           "Right=hold(kb,d);"
                           "Fast=hold(kb,lshift);};";

    Globals::InputActionManager->AddBindings(bindings);
}

void Noclip::OnDrawMenu(IImGuiRenderer* p_Renderer)
{
    if (ImGui::Checkbox(ICON_MD_SELF_IMPROVEMENT " Noclip", &m_IsNoclipEnabled))
    {
        if (m_IsNoclipEnabled)
        {
            ZHitman5* hitman = Globals::LevelManager->m_rHitman.m_pInterfaceRef;

            if (hitman)
            {
                m_PlayerPosition = hitman->GetSpatialEntityPtr()->GetWorldPosition();
            }
        }
    }
}

void Noclip::OnFrameUpdate(const SGameUpdateEvent& p_UpdateEvent)
{
    ZHitman5* hitman = Globals::LevelManager->m_rHitman.m_pInterfaceRef;

    if (!hitman)
    {
        return;
    }

    ZHM5MainCamera* mainCamera = hitman->m_rMainCamera.m_pInterfaceRef;

    if (!mainCamera)
    {
        return;
    }

    if (m_ToggleNoclipAction.Digital())
    {
        m_IsNoclipEnabled = !m_IsNoclipEnabled;

        if (m_IsNoclipEnabled)
        {
            m_PlayerPosition = hitman->GetSpatialEntityPtr()->GetWorldPosition();
        }
    }

    if (!m_IsNoclipEnabled)
    {
        return;
    }

    const SMatrix cameraTransform = mainCamera->GetObjectToWorldMatrix();
    float moveSpeed = 5.f;

    if (m_FastAction.Digital())
    {
        moveSpeed = 20.f;
    }

    const float gameTimeDelta = static_cast<float>(p_UpdateEvent.m_GameTimeDelta.ToSeconds());

    if (m_ForwardAction.Digital())
    {
        m_PlayerPosition += cameraTransform.Up * -moveSpeed * gameTimeDelta;
    }

    if (m_BackwardAction.Digital())
    {
        m_PlayerPosition += cameraTransform.Up * moveSpeed * gameTimeDelta;
    }

    if (m_LeftAction.Digital())
    {
        m_PlayerPosition += cameraTransform.Right * -moveSpeed * gameTimeDelta;
    }

    if (m_RightAction.Digital())
    {
        m_PlayerPosition += cameraTransform.Right * moveSpeed * gameTimeDelta;
    }

    hitman->GetSpatialEntityPtr()->SetWorldPosition(m_PlayerPosition);
}

DEFINE_THISCALL_MOD_DETOUR(Noclip, void, ZEntitySceneContext_ClearScene, ZEntitySceneContext* p_EntitySceneContext, bool p_FullyUnloadScene)
{
    m_IsNoclipEnabled = false;

    return { HookAction::Continue() };
}

DEFINE_HMASDK_MOD(Noclip);
