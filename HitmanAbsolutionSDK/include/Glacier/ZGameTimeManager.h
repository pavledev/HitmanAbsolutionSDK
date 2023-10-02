#pragma once

#include "IComponentInterface.h"
#include "ZGameTime.h"

#include <Common.h>

class HitmanAbsolutionSDK_API ZGameTimeManager : public IComponentInterface
{
public:
    const ZGameTime& GetRealTime() const;

private:
    PAD(0x2C);
    ZGameTime m_tRealTime; //0x30
    PAD(0x50);
};

static_assert(sizeof(ZGameTimeManager) == 0x88);
static_assert(alignof(ZGameTimeManager) == 0x8);
