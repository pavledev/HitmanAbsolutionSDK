#pragma once

#include <stdint.h>

#include "TIterator.h"
#include "TPair.h"
#include "SHashMapInfo.h"
#include "THashMapNode.h"

template <typename TKey, typename TValue>
class THashMapIterator : public TIterator<TPair<TKey, TValue>>
{
public:
	using NodeType = THashMapNode<TKey, TValue>;

	THashMapIterator(SHashMapInfo<TKey, TValue>* mapInfo, uint32_t bucket, NodeType* currentNode) :
		TIterator<TPair<TKey, TValue>>(&currentNode->m_data),
		m_info(mapInfo),
		m_nBucket(bucket)
	{
	}

	THashMapIterator(SHashMapInfo<TKey, TValue>* mapInfo) :
		TIterator<TPair<TKey, TValue>>(nullptr),
		m_info(mapInfo),
		m_nBucket(UINT32_MAX)
	{
	}

	NodeType& Node() const
	{
		NodeType* node = reinterpret_cast<NodeType*>(reinterpret_cast<char*>(this->m_pCurrent) - offsetof(NodeType, m_data));

		return *node;
	}

	NodeType& Node()
	{
		NodeType* node = reinterpret_cast<NodeType*>(reinterpret_cast<char*>(this->m_pCurrent) - offsetof(NodeType, m_data));

		return *node;
	}

	THashMapIterator& operator++()
	{
		uint32_t nextIndex = Node().m_iNext;

		if (nextIndex != UINT32_MAX)
		{
			this->m_pCurrent = &m_info->m_pNodes[nextIndex].m_data;

			return *this;
		}

		++m_nBucket;

		if (m_nBucket >= m_info->m_nBuckets)
		{
			m_nBucket = UINT32_MAX;
			this->m_pCurrent = nullptr;

			return *this;
		}

		while (m_info->m_pBuckets[m_nBucket] == UINT32_MAX)
		{
			++m_nBucket;

			if (m_nBucket >= m_info->m_nBuckets)
			{
				m_nBucket = UINT32_MAX;
				this->m_pCurrent = nullptr;

				return *this;
			}
		}

		this->m_pCurrent = &m_info->m_pNodes[m_info->m_pBuckets[m_nBucket]].m_data;

		return *this;
	}

private:
	SHashMapInfo<TKey, TValue>* m_info;
	unsigned int m_nBucket;
};
