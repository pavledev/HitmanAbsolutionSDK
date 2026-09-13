#pragma once

#include "TRedBlackTree.h"
#include "TBinaryTree.h"
#include "TPair.h"

template<typename TKey, typename TValue> class TMap
{
  public:
    using Iterator = TBinaryTreeIterator<TPair<TKey, TValue>>;
    using ConstIterator = TBinaryTreeIterator<const TPair<TKey, TValue>>;

    ConstIterator Begin() const
    {
        return m_container.Begin();
    }

    Iterator Begin()
    {
        return m_container.Begin();
    }

    ConstIterator End() const
    {
        return m_container.End();
    }

    Iterator End()
    {
        return m_container.End();
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

    bool Contains(const TKey& p_Key) const
    {
        return Find(p_Key) != End();
    }

    ConstIterator Find(const TKey& p_Key) const
    {
        const auto* node = Find(m_container.m_tree.m_pLeftRoot, p_Key);

        return node ? ConstIterator(&node->m_data) : end();
    }

    Iterator Find(const TKey& p_Key)
    {
        auto* node = Find(m_container.m_tree.m_pLeftRoot, p_Key);

        return node ? Iterator(&node->m_data) : end();
    }

    size_t Size() const
    {
        return m_container.m_nSize;
    }

  protected:
    const TBinaryTreeNode<TPair<TKey, TValue>>* Find(const TBinaryTreeNode<TPair<TKey, TValue>>* p_Root, const TKey& p_Key) const
    {
        if (!p_Root || p_Root->m_data.m_key == p_Key)
        {
            return p_Root;
        }

        if (p_Root->m_data.m_key < p_Key)
        {
            return Find(p_Root->m_pRight, p_Key);
        }

        return Find(p_Root->m_pLeft, p_Key);
    }

    TBinaryTreeNode<TPair<TKey, TValue>>* Find(TBinaryTreeNode<TPair<TKey, TValue>>* p_Root, const TKey& p_Key)
    {
        if (!p_Root || p_Root->m_data.m_key == p_Key)
        {
            return p_Root;
        }

        if (p_Root->m_data.m_key < p_Key)
        {
            return Find(p_Root->m_pRight, p_Key);
        }

        return Find(p_Root->m_pLeft, p_Key);
    }

    TRedBlackTree<TPair<TKey, TValue>> m_container;
};
