#pragma once

#include <Glacier/ZScene.h>
#include <Glacier/SGameUpdateEvent.h>
#include <Glacier/ZInput.h>

#include <IModInterface.h>

class Noclip : public IModInterface
{
  public:
    Noclip();
    ~Noclip();

    void Initialize() override;
    void OnEngineInitialized() override;
    void OnDrawMenu() override;

  private:
    void OnFrameUpdate(const SGameUpdateEvent& p_UpdateEvent);

    DECLARE_THISCALL_DETOUR_WITH_CONTEXT(
        Noclip, void, ZEntitySceneContext_ClearScene, ZEntitySceneContext* p_EntitySceneContext, bool p_FullyUnloadScene
    );

    bool m_IsNoclipEnabled;

    ZInputAction m_ToggleNoclipAction;
    ZInputAction m_ForwardAction;
    ZInputAction m_BackwardAction;
    ZInputAction m_LeftAction;
    ZInputAction m_RightAction;
    ZInputAction m_FastAction;

    float4 m_PlayerPosition;
};

DECLARE_MOD(Noclip)
