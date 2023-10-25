#pragma once

#include "TRedBlackTree.h"
#include "TBinaryTreeIterator.h"
#include "TBinaryTreeNode.h"
#include "TPair.h"

template <typename TKey, typename TValue>
class TMap
{
public:
    TBinaryTreeIterator<TPair<TKey, TValue>> Begin()
    {
        TBinaryTreeIterator<TPair<TKey, TValue>> result;

        if (m_container.m_tree.m_pLeftRoot)
        {
            result.m_pCurrent = &TBinaryTreeNode<TPair<TKey, TValue>>::GetNextNode(reinterpret_cast<TBinaryTreeNode<TPair<TKey, TValue>>*>(this))->m_data;
        }
        else
        {
            result.m_pCurrent = reinterpret_cast<TPair<TKey, TValue>*>(&m_container.m_nSize);
        }

        return result;
    }

    TBinaryTreeIterator<TPair<TKey, TValue>> End()
    {
        TBinaryTreeIterator<TPair<TKey, TValue>> result;

        result.m_pCurrent = reinterpret_cast<TPair<TKey, TValue>*>(&m_container.m_nSize);

        return result;
    }

    const TBinaryTreeIterator<TPair<TKey, TValue>> Begin() const
    {
        TBinaryTreeIterator<TPair<TKey, TValue>> result;

        if (m_container.m_tree.m_pLeftRoot)
        {
            result.m_pCurrent = &TBinaryTreeNode<TPair<TKey, TValue>>::GetNextNode((TBinaryTreeNode<TPair<TKey, TValue>>*)this)->m_data;
        }
        else
        {
            result.m_pCurrent = (TPair<TKey, TValue>*)(&m_container.m_nSize);
        }

        return result;
    }

    const TBinaryTreeIterator<TPair<TKey, TValue>> End() const
    {
        TBinaryTreeIterator<TPair<TKey, TValue>> result;

        result.m_pCurrent = (TPair<TKey, TValue>*)(&m_container.m_nSize);

        return result;
    }

    TBinaryTreeIterator<TPair<TKey, TValue>> Find(const TKey& key)
    {
        TBinaryTreeNode<TPair<TKey, TValue>>* node = Find(m_container.m_tree.m_pLeftRoot, key);

        if (node)
        {
            return &node->m_data;
        }

        return End();
    }

    TBinaryTreeNode<TPair<TKey, TValue>>* Find(TBinaryTreeNode<TPair<TKey, TValue>>* root, const TKey& key)
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

private:
    TRedBlackTree<TPair<TKey, TValue>> m_container;
};
