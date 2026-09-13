#pragma once

#include "ZDelegate.h"
#include "SGameUpdateEvent.h"
#include "IComponentInterface.h"

#include <Functions.h>

enum EPlayMode
{
    PLAYMODE_STOPPED = 1,
    PLAYMODE_PAUSED = 2,
    PLAYMODE_PLAYING = 3
};

class ZGameLoopManager : public IComponentInterface
{
  public:
    void RegisterForFrameUpdate(const ZDelegate<void __cdecl(SGameUpdateEvent const&)>& p_Callback, int p_Priority)
    {
        Functions::ZGameLoopManager_RegisterForFrameUpdate->Call(this, p_Callback, p_Priority);
    }

    void UnregisterForFrameUpdate(const ZDelegate<void __cdecl(SGameUpdateEvent const&)>& p_Callback)
    {
        Functions::ZGameLoopManager_UnregisterForFrameUpdate->Call(this, p_Callback);
    }

    void SetPlayMode(EPlayMode p_PlayMode)
    {
        Functions::ZGameLoopManager_SetPlayMode->Call(this, p_PlayMode);
    }
};
