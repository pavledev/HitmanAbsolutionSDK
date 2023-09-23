#pragma once

template <typename T>
class TArray
{
private:
    T* m_pStart;
    T* m_pEnd;
    T* m_pLast;
};
