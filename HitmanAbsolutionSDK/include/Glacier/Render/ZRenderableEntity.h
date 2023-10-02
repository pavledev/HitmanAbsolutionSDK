#pragma once

#include "ZBoundedEntity.h"

class HitmanAbsolutionSDK_API ZRenderableEntity : public ZBoundedEntity
{
public:
	virtual void OnVisibleOnScreenChanged(bool bVisibleOnScreen) = 0;

private:
	PAD(0x8);
};

static_assert(sizeof(ZRenderableEntity) == 0x60);
static_assert(alignof(ZRenderableEntity) == 0x4);
