#pragma once

template <typename T>
struct ZArrayRefData
{
    ZArrayRefData()
    {
        m_pStart = nullptr;
        m_pEnd = nullptr;
    }

    ZArrayRefData(T* pStart, T* pEnd)
    {
        this->m_pStart = pStart;
        this->m_pEnd = pEnd;
    }

    T* m_pStart;
    T* m_pEnd;
};
