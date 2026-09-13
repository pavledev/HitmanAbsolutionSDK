#pragma once

template<typename TElement, int TSize> struct ZFixedArrayData
{
    TElement m_pStart[TSize];
};

template<typename TElement, typename ZFixedArrayData> class TFixedArrayBase : public ZFixedArrayData
{
  public:
    uint32_t Size() const
    {
        return this->m_pEnd - this->m_pStart;
    }

    TElement* GetPointer()
    {
        return this->m_pStart;
    }

    TElement& operator[](uint32_t p_Index)
    {
        return this->m_pStart[p_Index];
    }

    const TElement& operator[](uint32_t p_Index) const
    {
        return this->m_pStart[p_Index];
    }
};

template<typename TElement, uint32_t TSize> class TFixedArray : public TFixedArrayBase<TElement, ZFixedArrayData<TElement, TSize>>
{
  public:
    TFixedArray() = default;

    TFixedArray(const TFixedArray& p_Other)
    {
        TFixedArray& array = *this;

        for (int i = 0; i < TSize; ++i)
        {
            array[i] = p_Other[i];
        }
    }

    TFixedArray& operator=(const TFixedArray& p_Other)
    {
        if (this != &p_Other)
        {
            TFixedArray& array = *this;

            for (int i = 0; i < TSize; ++i)
            {
                array[i] = p_Other[i];
            }
        }

        return *this;
    }

    uint32_t Size() const
    {
        return TSize;
    }

    TElement& operator[](uint32_t p_Index)
    {
        return this->m_pStart[p_Index];
    }

    const TElement& operator[](uint32_t p_Index) const
    {
        return this->m_pStart[p_Index];
    }
};

template<typename T> class ZArrayRefData
{
  public:
    ZArrayRefData()
    {
        m_pStart = nullptr;
        m_pEnd = nullptr;
    }

    ZArrayRefData(T* p_Start, T* p_End)
    {
        this->m_pStart = p_Start;
        this->m_pEnd = p_End;
    }

    T* m_pStart;
    T* m_pEnd;
};

template<typename T> class TArrayRef : public TFixedArrayBase<T, ZArrayRefData<T>>
{
  public:
    TArrayRef(T* p_Start, uint32_t p_Size)
    {
        this->m_pStart = p_Start;
        this->m_pEnd = &p_Start[p_Size];
    }

    TArrayRef(T* p_Start, T* p_End)
    {
        this->m_pStart = p_Start;
        this->m_pEnd = p_End;
    }

    TArrayRef(const TArrayRef& p_Other)
    {
        this->m_pStart = p_Other.m_pStart;
        this->m_pEnd = p_Other.m_pEnd;
    }
};
