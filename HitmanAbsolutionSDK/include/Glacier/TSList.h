#pragma once

#include "TIterator.h"

template<typename T> class TSListNode
{
  public:
    TSListNode* m_pNext;
    T m_data;
};

template<typename T> class TSListIterator : public TIterator<T>
{
  public:
    TSListIterator() {}

    TSListIterator(T* p_Element) : TIterator<T>(p_Element) {}

    const TSListNode<T>& Node() const
    {
        return *reinterpret_cast<const TSListNode<T>*>(
            reinterpret_cast<const char*>(this->m_pCurrent) - offsetof(TSListNode<T>, TSListNode<T>::m_data)
        );
    }

    TSListNode<T>& Node()
    {
        return const_cast<TSListNode<T>&>(static_cast<const TSListIterator&>(*this).Node());
    }

    TSListIterator& operator++()
    {
        this->m_pCurrent = &Node().m_pNext->m_data;
        return *this;
    }
};

template<typename T> class TSList
{
  public:
    using Iterator = TSListIterator<T>;
    using ConstIterator = TSListIterator<const T>;

    TSList() : m_nSize(0)
    {
        m_list.m_pFirst = nullptr;
    }

    ConstIterator Begin() const
    {
        return m_list.m_pFirst ? ConstIterator(&m_list.m_pFirst->m_data) : End();
    }

    Iterator Begin()
    {
        return m_list.m_pFirst ? Iterator(&m_list.m_pFirst->m_data) : End();
    }

    ConstIterator End() const
    {
        return ConstIterator(reinterpret_cast<const T*>(this + 1));
    }

    Iterator End()
    {
        return Iterator(reinterpret_cast<T*>(this + 1));
    }

    ConstIterator begin() const
    {
        return Begin();
    }

    Iterator begin()
    {
        return Begin();
    }

    ConstIterator end() const
    {
        return End();
    }

    Iterator end()
    {
        return End();
    }

    ConstIterator Find(const T& p_Element) const
    {
        for (auto it = Begin(); it != End(); ++it)
        {
            if (*it == p_Element)
            {
                return it;
            }
        }

        return End();
    }

    Iterator Find(const T& p_Element)
    {
        for (auto it = Begin(); it != End(); ++it)
        {
            if (*it == p_Element)
            {
                return it;
            }
        }

        return End();
    }

    const T& operator[](uint32_t p_Index) const
    {
        const TSListNode<T>* current = m_list.m_pFirst;

        for (uint32_t i = 0; i < p_Index; ++i)
        {
            current = current->m_pNext;
        }

        return current->m_data;
    }

    T& operator[](uint32_t p_Index)
    {
        TSListNode<T>* current = m_list.m_pFirst;

        for (uint32_t i = 0; i < p_Index; ++i)
        {
            current = current->m_pNext;
        }

        return current->m_data;
    }

    size_t Size() const
    {
        return m_nSize;
    }

    struct SFakeNode
    {
        TSListNode<T>* m_pFirst;
    };

    uint32_t m_nSize;
    SFakeNode m_list;
};
