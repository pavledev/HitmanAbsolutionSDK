#pragma once

#include "TIterator.h"
#include "Function.h"
#include "TBinaryTreeNode.h"

template <typename T>
class TBinaryTreeIterator : public TIterator<T>
{
public:
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
