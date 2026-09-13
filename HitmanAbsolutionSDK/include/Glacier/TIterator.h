#pragma once

template<typename T> class TIterator
{
  public:
    TIterator(T* p_Current)
    {
        m_pCurrent = p_Current;
    }

    const T& operator*() const
    {
        return *this->m_pCurrent;
    }

    T& operator*()
    {
        return *this->m_pCurrent;
    }

    const T* operator->() const
    {
        return this->m_pCurrent;
    }

    T* operator->()
    {
        return this->m_pCurrent;
    }

    const bool operator==(const TIterator& p_Other) const
    {
        return m_pCurrent == p_Other.m_pCurrent;
    }

    const bool operator!=(const TIterator& p_Other) const
    {
        return m_pCurrent != p_Other.m_pCurrent;
    }

    T* m_pCurrent = nullptr;
};
