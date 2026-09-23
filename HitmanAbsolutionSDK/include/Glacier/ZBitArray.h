#pragma once

#include "TArray.h"

class ZBitArray
{
  public:
    bool Get(uint32_t p_BitIndex) const
    {
        const uint32_t byteIndex = p_BitIndex >> 3;
        const uint32_t bitOffset = p_BitIndex & 7;

        return (1 << bitOffset) & m_aBytes[byteIndex];
    }

    TArray<uint8_t> m_aBytes;
    uint32_t m_nSize;
};
