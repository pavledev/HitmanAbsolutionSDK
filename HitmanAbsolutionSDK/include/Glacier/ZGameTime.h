#pragma once

#include <Common.h>

class ZGameTime
{
  public:
    double ToSeconds() const
    {
        return static_cast<double>(m_nTicks) / 1024.0 / 1024.0;
    }

    int64_t m_nTicks;
};
