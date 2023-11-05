#pragma once

#include "TFixedArrayBase.h"
#include "ZFixedArrayData.h"

template <typename TElement, unsigned int TSize>
class TFixedArray : public TFixedArrayBase<TElement, ZFixedArrayData<TElement, TSize>>
{
public:
    unsigned int Size() const
    {
        return TSize;
    }

    TElement& operator[](unsigned int nIndex)
    {
        return this->m_pStart[nIndex];
    }

    const TElement& operator[](unsigned int nIndex) const
    {
        return this->m_pStart[nIndex];
    }
};
