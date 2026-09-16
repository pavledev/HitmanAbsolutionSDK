#pragma once

#include <IModInterface.h>

class SkipIntro : public IModInterface
{
  public:
    void Initialize() override;

  private:
    DECLARE_THISCALL_DETOUR_WITH_CONTEXT(SkipIntro, void, ZMenuManager_SetStartupState, ZMenuManager* p_MenuManager, EMenuStartupState p_NewState);

    DECLARE_THISCALL_DETOUR_WITH_CONTEXT(
        SkipIntro, ZRuntimeResourceID*, ZLevelSelectManager_GetBootMovie, ZLevelSelectManager* p_LevelSelectManager, ZRuntimeResourceID& p_Result
    );
};

DECLARE_MOD(SkipIntro)
