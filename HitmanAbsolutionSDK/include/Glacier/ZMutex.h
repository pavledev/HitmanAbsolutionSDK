#pragma once

#include <Common.h>

class ZMutex
{
  public:
    uint32_t m_impl[6];
    uint32_t m_nUniqueID;
};
