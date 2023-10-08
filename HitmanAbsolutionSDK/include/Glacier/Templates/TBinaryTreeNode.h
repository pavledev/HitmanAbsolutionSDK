#pragma once

template <typename T>
class TBinaryTreeNode
{
public:
    static TBinaryTreeNode* GetNextNode(TBinaryTreeNode* pNode)
    {
        TBinaryTreeNode* result = pNode->m_pRight;

        if (result)
        {
            for (TBinaryTreeNode* i = result->m_pLeft; i; i = i->m_pLeft)
            {
                result = i;
            }
        }
        else
        {
            result = pNode->m_pParent;

            if (result)
            {
                if (result->m_pLeft == pNode)
                {
                    return result;
                }

                do
                {
                    if (result->m_pLeft == pNode)
                    {
                        break;
                    }

                    pNode = result;
                    result = result->m_pParent;
                }
                while (result);
            }

            result = pNode->m_pParent;

            if (!result)
            {
                return pNode;
            }
        }

        return result;
    }

    const T& Data() const
    {
        return m_data;
    }

    T& Data()
    {
        return m_data;
    }

public:
    int m_nBalance;
    TBinaryTreeNode* m_pParent;
    TBinaryTreeNode* m_pLeft;
    TBinaryTreeNode* m_pRight;
    T m_data;
};
