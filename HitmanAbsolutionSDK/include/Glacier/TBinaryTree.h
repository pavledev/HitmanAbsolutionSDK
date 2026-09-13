#pragma once

#include <cstdint>

#include "TIterator.h"

template<typename T> class TBinaryTreeNode
{
  public:
    static TBinaryTreeNode* GetNextNode(TBinaryTreeNode* p_Node)
    {
        // Case 1: node has right child -> go to leftmost node in right subtree
        TBinaryTreeNode* currentNode = p_Node;

        if (currentNode->m_pRight)
        {
            TBinaryTreeNode* successorNode = currentNode->m_pRight;

            while (successorNode->m_pLeft)
            {
                successorNode = successorNode->m_pLeft;
            }

            return successorNode;
        }

        // Case 2: no right child -> go up until the current node is a left child
        TBinaryTreeNode* parentNode = currentNode->m_pParent;

        if (!parentNode)
        {
            return currentNode;
        }

        while (parentNode && currentNode != parentNode->m_pLeft)
        {
            currentNode = parentNode;
            parentNode = parentNode->m_pParent;
        }

        return parentNode;
    }

    int32_t m_nBalance;
    TBinaryTreeNode* m_pParent;
    TBinaryTreeNode* m_pLeft;
    TBinaryTreeNode* m_pRight;
    T m_data;
};

template<typename T> class TBinaryTreeIterator : public TIterator<T>
{
  public:
    const TBinaryTreeNode<T>& Node() const
    {
        return *reinterpret_cast<const TBinaryTreeNode<T>*>(reinterpret_cast<const char*>(this->m_pCurrent) - offsetof(TBinaryTreeNode<T>, m_data));
    }

    TBinaryTreeNode<T>& Node()
    {
        return const_cast<TBinaryTreeNode<T>&>(static_cast<const TBinaryTreeIterator&>(*this).Node());
    }

    TBinaryTreeIterator& operator++()
    {
        this->m_pCurrent = &TBinaryTreeNode<T>::GetNextNode(&Node())->m_data;
        return *this;
    }
};

template<typename T> class TBinaryTree
{
  public:
    using Iterator = TBinaryTreeIterator<T>;
    using ConstIterator = TBinaryTreeIterator<const T>;

    ConstIterator Begin() const
    {
        if (m_tree.m_pLeftRoot)
        {
            using NodeType = TBinaryTreeNode<std::remove_const_t<T>>;

            NodeType* fakeRootNode = reinterpret_cast<NodeType*>(const_cast<TBinaryTree<T>*>(this));
            NodeType* firstNode = NodeType::GetNextNode(fakeRootNode);

            return ConstIterator(&firstNode->m_data);
        }

        return ConstIterator(reinterpret_cast<const T*>(&m_nSize));
    }

    Iterator Begin()
    {
        if (m_tree.m_pLeftRoot)
        {
            using NodeType = TBinaryTreeNode<std::remove_const_t<T>>;

            NodeType* fakeRootNode = reinterpret_cast<NodeType*>(this);
            NodeType* firstNode = NodeType::GetNextNode(fakeRootNode);

            return Iterator(&firstNode->m_data);
        }

        return Iterator(reinterpret_cast<T*>(&m_nSize));
    }

    ConstIterator End() const
    {
        return ConstIterator(reinterpret_cast<const T*>(&m_nSize));
    }

    Iterator End()
    {
        return Iterator(reinterpret_cast<T*>(&m_nSize));
    }

    size_t Size() const
    {
        return m_nSize;
    }

    bool Contains(const T& p_Value) const
    {
        return Find(p_Value) != End();
    }

    ConstIterator Find(const T& p_Value) const
    {
        auto* node = Find(m_tree.m_pLeftRoot, p_Value);

        return node ? ConstIterator(&node->m_data) : End();
    }

    Iterator Find(const T& p_Value)
    {
        auto* node = Find(m_tree.m_pLeftRoot, p_Value);

        return node ? Iterator(&node->m_data) : End();
    }

  private:
    TBinaryTreeNode<T>* Find(TBinaryTreeNode<T>* p_Root, const T& p_Value) const
    {
        if (!p_Root || p_Root->m_data == p_Value)
        {
            return p_Root;
        }

        if (p_Root->m_data < p_Value)
        {
            return Find(p_Root->m_pRight, p_Value);
        }

        return Find(p_Root->m_pLeft, p_Value);
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
