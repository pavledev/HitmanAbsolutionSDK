#pragma once

#include <IModInterface.h>

class SkipIntro : public IModInterface
{
  public:
    void Initialize() override;

  private:
    DECLARE_THISCALL_MOD_DETOUR(SkipIntro, void, ZMenuManager_SetStartupState, ZMenuManager* p_MenuManager, EMenuStartupState p_NewState);

    DECLARE_THISCALL_MOD_DETOUR(
        SkipIntro, ZRuntimeResourceID*, ZLevelSelectManager_GetBootMovie, ZLevelSelectManager* p_LevelSelectManager, ZRuntimeResourceID& p_Result
    );
};

DECLARE_HMASDK_MOD(SkipIntro)
