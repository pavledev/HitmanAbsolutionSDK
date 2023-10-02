#pragma once

#include <Common.h>

class HitmanAbsolutionSDK_API ZGameTime
{
public:
    long long GetTicks() const;

private:
    long long m_nTicks;
};
