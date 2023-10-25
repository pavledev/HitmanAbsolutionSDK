#pragma once

template <typename T>
class TListNode
{
private:
    TListNode<T>* m_pNext;
    TListNode<T>* m_pPrevious;
    T m_data;
};
