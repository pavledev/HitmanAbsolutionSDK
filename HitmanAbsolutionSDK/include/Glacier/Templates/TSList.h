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
		return TSListIterator<T>(nullptr);
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

private:
	struct SFakeNode
	{
		TSListNode<T>* m_pFirst;
	};

	unsigned int m_nSize;
	SFakeNode m_list;
};
