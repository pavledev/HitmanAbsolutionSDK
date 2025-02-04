#pragma once

#include "IComponentInterface.h"
#include "ZGameTime.h"

#include <Common.h>

class HitmanAbsolutionSDK_API ZGameTimeManager : public IComponentInterface
{
public:
    const ZGameTime& GetRealTime() const;

private:
	long long m_nTicksPerSecond;
	long long m_nLastTimeTicks;
	ZGameTime m_tGameTime;
	ZGameTime m_tGameTimePrev;
	ZGameTime m_tGameTimeDelta;
	ZGameTime m_tRealTime;
	ZGameTime m_tRealTimePrev;
	ZGameTime m_tRealTimeDelta;
	float m_fGameTimeMultiplier;
	float m_fDebugTimeMultiplier;
	ZGameTime m_FrameWait;
	ZGameTime m_FrameStep;
	ZGameTime m_FrameRemain;
	bool m_bPaused;
	unsigned int m_nFrameCount;
	bool m_bForcedTimeStep;
	double m_fForcedTimeStep;
	double m_fForcedTimeTarget;
};

static_assert(sizeof(ZGameTimeManager) == 0x88);
static_assert(alignof(ZGameTimeManager) == 0x8);
