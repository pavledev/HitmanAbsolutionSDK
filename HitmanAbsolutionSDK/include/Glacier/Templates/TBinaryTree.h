#pragma once

#include "TBinaryTreeIterator.h"
#include "TBinaryTreeNode.h"

template <typename T>
class TBinaryTree
{
public:
    TBinaryTreeIterator<T> Begin()
    {
        TBinaryTreeIterator<T> result;

        if (m_tree.m_pLeftRoot)
        {
            result.m_pCurrent = &TBinaryTreeNode<T>::GetNextNode(reinterpret_cast<TBinaryTreeNode<T>*>(this))->m_data;
        }
        else
        {
            result.m_pCurrent = reinterpret_cast<T*>(&m_nSize);
        }

        return result;
    }

    TBinaryTreeIterator<T> End()
    {
        TBinaryTreeIterator<T> result;

        result.m_pCurrent = reinterpret_cast<T*>(&m_nSize);

        return result;
    }

    TBinaryTreeIterator<T> Find(const T& key)
    {
        TBinaryTreeNode<T>* node = Find(m_tree.m_pLeftRoot, key);

        if (node)
        {
            return &node->m_data;
        }

        return End();
    }

    TBinaryTreeIterator<T> Find(TBinaryTreeNode<T>* root, const T& key)
    {
        if (root == nullptr || root->m_data == key)
        {
            return root;
        }

        if (root->m_data < key)
        {
            return Find(root->m_pRight, key);
        }

        return Find(root->m_pLeft, key);
    }

    bool IsEmpty()
    {
        return m_nSize == 0;
    }

public:
    struct SFakeTreeNode
    {
        int m_reserved1;
        TBinaryTreeNode<T>* m_pNULL;
        TBinaryTreeNode<T>* m_pRightRoot;
        TBinaryTreeNode<T>* m_pLeftRoot;
    };

    SFakeTreeNode m_tree;
    int m_nSize;
};
