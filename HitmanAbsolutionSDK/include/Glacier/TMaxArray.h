#pragma once

#include <cstdint>

#include "TAlignedType.h"

template<typename TElement, int TCapacity> struct ZMaxArrayData
{
    union
    {
        uint8_t m_data[TCapacity * sizeof(TElement)];
        TAlignedType<alignof(TElement)> alignDummy;
    };
};

template<typename TElement, typename TStorage> class TMaxArrayBase : public TStorage
{
  public:
    uint32_t Size()
    {
        return this->m_nSize;
    }

    const TElement& operator[](size_t p_Index) const
    {
        return reinterpret_cast<const TElement*>(this->m_data)[p_Index];
    }

    TElement& operator[](size_t p_Index)
    {
        return reinterpret_cast<TElement*>(this->m_data)[p_Index];
    }

    TElement* begin()
    {
        return reinterpret_cast<TElement*>(this->m_data);
    }

    const TElement* begin() const
    {
        return reinterpret_cast<const TElement*>(this->m_data);
    }

    TElement* end()
    {
        return begin() + m_nSize;
    }

    const TElement* end() const
    {
        return begin() + m_nSize;
    }

    uint32_t m_nSize;
};

template<typename TElement, int TCapacity> class TMaxArray : public TMaxArrayBase<TElement, ZMaxArrayData<TElement, TCapacity>>
{};
