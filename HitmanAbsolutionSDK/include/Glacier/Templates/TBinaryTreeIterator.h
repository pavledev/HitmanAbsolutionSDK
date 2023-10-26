#pragma once

#include <cstddef>

#include "TIterator.h"
#include "TBinaryTreeNode.h"

template <typename T>
class TBinaryTreeIterator : public TIterator<T>
{
public:
    const TBinaryTreeNode<T>& Node() const
    {
        const TBinaryTreeNode<T>* node = reinterpret_cast<const TBinaryTreeNode<T>*>(reinterpret_cast<const char*>(this->m_pCurrent) - offsetof(TBinaryTreeNode<T>, TBinaryTreeNode<T>::m_data));

        return *node;
    }

    TBinaryTreeNode<T>& Node()
    {
        TBinaryTreeNode<T>* node = reinterpret_cast<TBinaryTreeNode<T>*>(reinterpret_cast<char*>(this->m_pCurrent) - offsetof(TBinaryTreeNode<T>, TBinaryTreeNode<T>::m_data));

        return *node;
    }

    TBinaryTreeIterator& operator++()
    {
        TBinaryTreeNode<T>* node = reinterpret_cast<TBinaryTreeNode<T>*>(reinterpret_cast<char*>(this->m_pCurrent) - offsetof(TBinaryTreeNode<T>, TBinaryTreeNode<T>::m_data));

        this->m_pCurrent = &TBinaryTreeNode<T>::GetNextNode(node)->m_data;

        return *this;
    }
};
