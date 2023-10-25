#pragma once

#include "IComponentInterface.h"
#include "Templates/TEntityRef.h"
#include "Player/ZHitman5.h"
#include "SSceneParameters.h"
#include "Outfit/ZOutfitManagerEntity.h"

#include <Common.h>

class HitmanAbsolutionSDK_API ZLevelManager : public IComponentInterface
{
public:
	const SSceneParameters& GetSceneParameters() const;
	SSceneParameters& GetSceneParameters();
	const TEntityRef<ZHitman5>& GetHitman() const;
	TEntityRef<ZOutfitManagerEntity> GetOutfitManager() const;

private:
	SSceneParameters m_SceneTransitionData; //0x4
	PAD(0x10);
	TEntityRef<ZHitman5> m_rHitman; //0x44
	PAD(0x10);
	TEntityRef<ZOutfitManagerEntity> m_pOutfitManager; //0x5C
	PAD(0xDC);
};

static_assert(sizeof(ZLevelManager) == 0x140);
static_assert(alignof(ZLevelManager) == 0x4);