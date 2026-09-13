#pragma once

#include <cstdint>

template<uint32_t T> class alignas(T) TAlignedType
{
  public:
    char dummy;
};
