#pragma once

#include "TListNode.h"

template <typename T>
class TList
{
public:
    unsigned int Size()
    {
        return this->m_nSize;
    }

private:
    struct ZFakeListNode
    {
        TListNode<T>* m_pFirst;
        TListNode<T>* m_pLast;
    };

    unsigned int m_nSize;
    ZFakeListNode m_list;
};
