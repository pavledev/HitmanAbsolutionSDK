#pragma once

#include "TFixedArrayBase.h"
#include "ZFixedArrayData.h"

template <typename TElement, unsigned int TSize>
class TFixedArray : public TFixedArrayBase<TElement, ZFixedArrayData<TElement, TSize>>
{
public:
    TFixedArray() = default;

    TFixedArray(const TFixedArray& other)
    {
        TFixedArray& array = *this;

        for (int i = 0; i < TSize; ++i)
        {
            array[i] = other[i];
        }
    }

    TFixedArray& operator=(const TFixedArray& other)
    {
        if (this != &other)
        {
            TFixedArray& array = *this;

            for (int i = 0; i < TSize; ++i)
            {
                array[i] = other[i];
            }
        }

        return *this;
    }

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
