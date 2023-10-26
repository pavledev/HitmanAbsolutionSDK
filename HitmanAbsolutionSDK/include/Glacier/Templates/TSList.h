#pragma once

#include "TSListIterator.h"
#include "TSListNode.h"

template <typename T>
class TSList
{
public:
	TSList() : m_nSize(0)
	{
		m_list.m_pFirst = nullptr;
	}

	TSListIterator<T> Begin()
	{
		return TSListIterator<T>(&m_list.m_pFirst->m_data);
	}

	TSListIterator<T> End()
	{
		return TSListIterator<T>(reinterpret_cast<T*>(this + 1));
	}

	TSListIterator<T> Find(const T& element)
	{
		TSListIterator<T> result;
		TSListNode<T>* listNode = m_list.m_pFirst;

		while (listNode)
		{
			if (listNode->m_data == element)
			{
				result.m_pCurrent = &listNode->m_data;

				return result;
			}

			listNode = listNode->m_pNext;
		}

		return result;
	}

	T& operator[](unsigned int index)
	{
		TSListNode<T>* current = m_list.m_pFirst;

		for (unsigned int i = 0; i < index; ++i)
		{
			current = current->m_pNext;
		}

		return current->m_data;
	}

	const T& operator[](unsigned int index) const
	{
		TSListNode<T>* current = m_list.m_pFirst;

		for (unsigned int i = 0; i < index; ++i)
		{
			current = current->m_pNext;
		}

		return current->m_data;
	}

private:
	struct SFakeNode
	{
		TSListNode<T>* m_pFirst;
	};

	unsigned int m_nSize;
	SFakeNode m_list;
};
