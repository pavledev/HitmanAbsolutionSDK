#pragma once

#include "IComponentInterface.h"
#include "Templates/TEntityRef.h"
#include "ZHitman5.h"

#include <Common.h>

class HitmanAbsolutionSDK_API ZLevelManager : public IComponentInterface
{
public:
	const TEntityRef<ZHitman5>& GetHitman() const;

private:
	PAD(0x40);
	TEntityRef<ZHitman5> m_rHitman; //0x44
	PAD(0xF4);
};

static_assert(sizeof(ZLevelManager) == 0x140);
static_assert(alignof(ZLevelManager) == 0x4);