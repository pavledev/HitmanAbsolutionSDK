#pragma once

#include "ZMath.h"

class ZColor
{
  public:
    static SVector4 UnpackUnsigned(const uint32_t p_PackedVector4)
    {
        const uint32_t alpha = (p_PackedVector4 >> 24) & 0xFF;
        const uint32_t blue = (p_PackedVector4 >> 16) & 0xFF;
        const uint32_t green = (p_PackedVector4 >> 8) & 0xFF;
        const uint32_t red = p_PackedVector4 & 0xFF;

        return SVector4(
            static_cast<float>(red) / 255.f, static_cast<float>(green) / 255.f, static_cast<float>(blue) / 255.f, static_cast<float>(alpha) / 255.f
        );
    }
};
