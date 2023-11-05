#pragma once

#include "TDefaultHashMapPolicy.h"
#include "THashMapIterator.h"

template <typename TKey, typename TValue, typename THashMapPolicy = TDefaultHashMapPolicy<TKey, TValue>>
class THashMap
{
public:
    THashMapIterator<TKey, TValue> Begin() const
    {
        if (m_info.m_nBuckets == 0 || m_info.m_pBuckets[0] == UINT32_MAX)
        {
            return THashMapIterator<TKey, TValue>(&m_info);
        }

        return THashMapIterator<TKey, TValue>(&m_info, 0, &m_info.m_pNodes[m_info.m_pBuckets[0]]);
    }

    THashMapIterator<TKey, TValue> End() const
    {
        return THashMapIterator<TKey, TValue>(&m_info);
    }

    THashMapIterator<TKey, TValue> Begin()
    {
        if (m_info.m_nBuckets == 0 || m_info.m_pBuckets[0] == UINT32_MAX)
        {
            return THashMapIterator<TKey, TValue>(&m_info);
        }

        return THashMapIterator<TKey, TValue>(&m_info, 0, &m_info.m_pNodes[m_info.m_pBuckets[0]]);
    }

    THashMapIterator<TKey, TValue> End()
    {
        return THashMapIterator<TKey, TValue>(&m_info);
    }

    unsigned int Size() const
    {
        return m_nSize;
    }

    THashMapIterator<TKey, TValue> Find(const TKey& key)
    {
        if (!m_info.m_pBuckets)
        {
            return THashMapIterator<TKey, TValue>(&m_info);
        }

        const unsigned int hash = THashMapPolicy::GetHashCode(key);
        uint32_t bucket = hash % m_info.m_nBuckets;
        int nodeIndex = m_info.m_pBuckets[bucket];

        if (nodeIndex == UINT32_MAX)
        {
            return THashMapIterator<TKey, TValue>(&m_info);
        }

        THashMapNode<TKey, TValue>* node = nullptr;

        while (true)
        {
            node = &m_info.m_pNodes[nodeIndex];

            if (node->m_data.Key() == key)
            {
                break;
            }

            if (node->m_iNext == UINT32_MAX)
            {
                return THashMapIterator<TKey, TValue>(&m_info);
            }

            nodeIndex = node->m_iNext;
        }

        return THashMapIterator<TKey, TValue>(&m_info, bucket, node);
    }

    TValue& operator[](const TKey& key) const
    {
        if (!m_info.m_pBuckets)
        {
            return TValue{};
        }

        const unsigned int hash = THashMapPolicy::GetHashCode(key);
        uint32_t bucket = hash % m_info.m_nBuckets;
        int nodeIndex = m_info.m_pBuckets[bucket];

        if (nodeIndex == UINT32_MAX)
        {
            return TValue{};
        }

        THashMapNode<TKey, TValue>* node = nullptr;

        while (true)
        {
            node = &m_info.m_pNodes[nodeIndex];

            if (node->m_data.Key() == key)
            {
                break;
            }

            if (node->m_iNext == UINT32_MAX)
            {
                return TValue{};
            }

            nodeIndex = node->m_iNext;
        }

        return node->m_data.Value();
    }

    bool Contains(const TKey& key) const
    {
        if (!m_info.m_pBuckets)
        {
            return false;
        }

        const auto hash = THashMapPolicy::GetHashCode(key);

        uint32_t bucket = hash % m_info.m_nBuckets;
        auto nodeIndex = m_info.m_pBuckets[bucket];

        if (nodeIndex == UINT32_MAX)
        {
            return false;
        }

        THashMapNode<TKey, TValue>* node = nullptr;

        while (true)
        {
            node = &m_info.m_pNodes[nodeIndex];

            if (node->m_data.Key() == key)
            {
                break;
            }

            if (node->m_iNext == UINT32_MAX)
            {
                return false;
            }

            nodeIndex = node->m_iNext;
        }

        return true;
    }

private:
    unsigned int m_nSize;
    int m_iFree;
    SHashMapInfo<TKey, TValue> m_info;
};
