#include "SkipIntro.h"

#include <Glacier/ZMenu.h>
#include <Glacier/ZResource.h>

#include <Hooks.h>

void SkipIntro::Initialize()
{
    Hooks::ZMenuManager_SetStartupState->AddDetour(this, &SkipIntro::ZMenuManager_SetStartupState);
    Hooks::ZLevelSelectManager_GetBootMovie->AddDetour(this, &SkipIntro::ZLevelSelectManager_GetBootMovie);
}

DEFINE_THISCALL_DETOUR_WITH_CONTEXT(SkipIntro, void, ZMenuManager_SetStartupState, ZMenuManager* p_MenuManager, EMenuStartupState p_NewState)
{
    if (p_NewState == EMenuStartupState::EMENUPHASE_INTRO_MOVIE)
    {
        p_Hook->CallOriginal(p_MenuManager, EMenuStartupState::EMENUPHASE_MAIN_MENU);

        return { HookAction::Return() };
    }

    return { HookAction::Continue() };
}

DEFINE_THISCALL_DETOUR_WITH_CONTEXT(
    SkipIntro, ZRuntimeResourceID*, ZLevelSelectManager_GetBootMovie, ZLevelSelectManager* p_LevelSelectManager, ZRuntimeResourceID& p_Result
)
{
    p_Result = ZRuntimeResourceID();

    return { HookAction::Return(), &p_Result };
}

DEFINE_MOD(SkipIntro);
