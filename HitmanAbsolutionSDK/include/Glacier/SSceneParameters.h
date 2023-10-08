#pragma once

#include "ZString.h"
#include "ECheckpointGameMode.h"
#include "STokenID.h"

struct SSceneParameters
{
	ZString sSceneResource;
	ECheckpointGameMode eGameMode;
	STokenID BonusWeapon;
	STokenID BonusOutfit;
	int nCheckpointIndex;
	bool bRestoring;
	bool bUseSaveGame;
	STokenID sStartCheckpointID;
	bool bGameCompleted;
};
