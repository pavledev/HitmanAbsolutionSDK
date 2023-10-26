#pragma once

#include <cstddef>

#include "TIterator.h"
#include "TSListNode.h"

template <typename T>
class TSListIterator : public TIterator<T>
{
public:
	TSListIterator()
	{
	}

	TSListIterator(T* pElement) : TIterator<T>(pElement)
	{
	}

	TSListNode<T>& Node()
	{
		TSListNode<T>* node = reinterpret_cast<TSListNode<T>*>(reinterpret_cast<char*>(this->m_pCurrent) - offsetof(TSListNode<T>, TSListNode<T>::m_data));

		return *node;
	}

	TSListIterator& operator++()
	{
		TSListNode<T>* node = reinterpret_cast<TSListNode<T>*>(reinterpret_cast<char*>(this->m_pCurrent) - offsetof(TSListNode<T>, TSListNode<T>::m_data));

		this->m_pCurrent = &node->m_pNext->m_data;

		return *this;
	}
};
