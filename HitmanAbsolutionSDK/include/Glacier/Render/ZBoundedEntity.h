#pragma once

#include "ZSpatialEntity.h"

class HitmanAbsolutionSDK_API ZBoundedEntity : public ZSpatialEntity
{
public:
	virtual void UpdateBounds() = 0;

private:
	PAD(0x18);
};

static_assert(sizeof(ZBoundedEntity) == 0x58);
static_assert(alignof(ZBoundedEntity) == 0x4);
