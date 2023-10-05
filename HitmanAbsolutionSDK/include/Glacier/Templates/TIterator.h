#pragma once

template <typename T>
class TIterator
{
public:
    TIterator()
    {
        m_pCurrent = nullptr;
    }

    TIterator(T* p)
    {
        m_pCurrent = p;
    }

    const bool operator==(const TIterator& rhs) const
    {
        return m_pCurrent == rhs.m_pCurrent;
    }

    const bool operator!=(const TIterator& rhs) const
    {
        return m_pCurrent != rhs.m_pCurrent;
    }

    T* operator->()
    {
        return m_pCurrent;
    }

    T& operator*()
    {
        return *m_pCurrent;
    }

public:
    T* m_pCurrent;
};
