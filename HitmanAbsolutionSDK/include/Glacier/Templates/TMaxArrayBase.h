#pragma once

template <typename A, typename B>
class TMaxArrayBase : public B
{
public:
    unsigned int Size()
    {
        return this->m_nSize;
    }

    A* operator[](unsigned int nIndex)
    {
        return (A*)((char*)this + sizeof(A) * nIndex);
    }

protected:
    unsigned int m_nSize;
};
