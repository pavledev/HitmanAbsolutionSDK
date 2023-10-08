#pragma once

#include "TRedBlackTree.h"

template <typename T>
class TSet : public TRedBlackTree<T>
{
public:
    TBinaryTreeIterator<T> Begin()
    {
        TBinaryTreeIterator<T> result;

        if (this->m_tree.m_pLeftRoot)
        {
            result.m_pCurrent = &TBinaryTreeNode<T>::GetNextNode(reinterpret_cast<TBinaryTreeNode<T>*>(this))->m_data;
        }
        else
        {
            result.m_pCurrent = reinterpret_cast<T*>(&this->m_nSize);
        }

        return result;
    }

    TBinaryTreeIterator<T> End()
    {
        TBinaryTreeIterator<T> result;

        result.m_pCurrent = reinterpret_cast<T*>(&this->m_nSize);

        return result;
    }

    const TBinaryTreeIterator<const T> Begin() const
    {
        TBinaryTreeIterator<const T> result;

        if (this->m_tree.m_pLeftRoot)
        {
            result.m_pCurrent = &TBinaryTreeNode<T>::GetNextNode((TBinaryTreeNode<T>*)this)->m_data;
        }
        else
        {
            result.m_pCurrent = reinterpret_cast<const T*>(&this->m_nSize);
        }

        return result;
    }

    const TBinaryTreeIterator<const T> End() const
    {
        TBinaryTreeIterator<const T> result;

        result.m_pCurrent = reinterpret_cast<const T*>(&this->m_nSize);

        return result;
    }
};
