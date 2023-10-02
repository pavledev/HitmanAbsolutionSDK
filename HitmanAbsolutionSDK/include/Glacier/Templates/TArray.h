#pragma once

template <class T>
class TArray
{
public:
    TArray() :
        m_pStart(nullptr),
        m_pEnd(nullptr),
        m_pLast(nullptr)
    {
    }

    size_t Size() const
    {
        if (HasInlineFlag())
        {
            return m_nInlineCount;
        }

        return m_pEnd - m_pStart;
    }

    size_t Capacity() const
    {
        if (HasInlineFlag())
        {
            return m_nInlineCapacity;
        }

        return m_pLast - m_pStart;
    }

    const T& operator[](size_t index) const
    {
        const T* begin = Begin();

        return begin[index];
    }

    T& operator[](size_t index)
    {
        T* begin = Begin();

        return begin[index];
    }

    T* Begin()
    {
        if (HasInlineFlag())
        {
            return reinterpret_cast<T*>(&m_pStart);
        }

        return m_pStart;
    }

    T* End()
    {
        if (HasInlineFlag())
        {
            return Begin() + m_nInlineCount;
        }

        return m_pEnd;
    }

    const T* Begin() const
    {
        if (HasInlineFlag())
        {
            return reinterpret_cast<const T*>(&m_pStart);
        }

        return m_pStart;
    }

    const T* End() const
    {
        if (HasInlineFlag())
        {
            return Begin() + m_nInlineCount;
        }

        return m_pEnd;
    }

    const bool HasInlineFlag() const
    {
        return (m_nFlags & 0x4000000000000000) != 0;
    }

private:
    T* m_pStart;
    T* m_pEnd;

    union
    {
        T* m_pLast;
        unsigned long long m_nFlags;

        struct
        {
            unsigned char m_nInlineCount;
            unsigned char m_nInlineCapacity;
        };
    };
};
