#pragma once

#include <Common.h>

class HitmanAbsolutionSDK_API ZGameTime
{
public:
    long long GetTicks() const;
    double ToSeconds() const;

private:
    long long m_nTicks;
};
