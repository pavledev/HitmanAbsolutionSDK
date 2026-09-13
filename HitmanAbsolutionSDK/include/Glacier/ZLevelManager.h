#pragma once

#include "IComponentInterface.h"
#include "ZEntity.h"
#include "ZHitman5.h"
#include "ZOutfit.h"

enum ECheckpointGameMode
{
    eCGM_CONTRACT = 0,
    eCGM_STORYMODE = 1,
    eCGM_MAINMENU = 2,
    eCGM_DLC = 3,
    eCGM_BENCHMARK = 4
};

struct SSceneParameters
{
    ZString sSceneResource;
    ECheckpointGameMode eGameMode;
    STokenID BonusWeapon;
    STokenID BonusOutfit;
    int32_t nCheckpointIndex;
    bool bRestoring;
    bool bUseSaveGame;
    STokenID sStartCheckpointID;
    bool bGameCompleted;
};

class ZLevelManager : public IComponentInterface
{
  public:
    SSceneParameters m_SceneTransitionData;            // 0x4
    PAD(0x10);                                         // 0x34
    TEntityRef<ZHitman5> m_rHitman;                    // 0x44
    PAD(0x10);                                         // 0x4C
    TEntityRef<ZOutfitManagerEntity> m_pOutfitManager; // 0x5C
    PAD(0xDC);                                         // 0x64
};

static_assert(sizeof(ZLevelManager) == 0x140);
static_assert(alignof(ZLevelManager) == 0x4);
