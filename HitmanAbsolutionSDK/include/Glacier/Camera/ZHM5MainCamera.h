#pragma once

#include "ZCameraEntity.h"

class HitmanAbsolutionSDK_API ZHM5MainCamera : public ZCameraEntity
{
public:
	void SetCameraDirection(const float4& vLookTo);

private:
	PAD(0x3C0);
};

static_assert(sizeof(ZHM5MainCamera) == 0x550);
static_assert(alignof(ZHM5MainCamera) == 0x10);
