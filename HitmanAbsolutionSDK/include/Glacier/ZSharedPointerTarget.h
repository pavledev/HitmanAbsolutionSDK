#pragma once

#include "ISharedPointerTarget.h"

class HitmanAbsolutionSDK_API ZSharedPointerTarget : public ISharedPointerTarget
{
private:
	long m_iRefCount;
};
