#pragma once

template<typename T> class TListNode
{
    TListNode<T>* m_pNext;
    TListNode<T>* m_pPrevious;
    T m_data;
};

template<typename T> class TList
{
  public:
    struct ZFakeListNode
    {
        TListNode<T>* m_pFirst;
        TListNode<T>* m_pLast;
    };

    uint32_t m_nSize;
    ZFakeListNode m_list;
};
