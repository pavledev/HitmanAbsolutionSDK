#pragma once

#include <cstdint>

#include "TIterator.h"
#include "TPair.h"

#include <Hash.h>

template<typename TKey, typename TValue> struct THashMapNode
{
    int32_t m_iNext;
    TPair<TKey, TValue> m_data;
};

template<typename TKey, typename TValue> struct SHashMapInfo
{
    uint32_t m_nBuckets;
    int32_t* m_pBuckets;
    THashMapNode<TKey, TValue>* m_pNodes;
};

template<typename T> class TDefaultHashMapPolicy
{
  public:
    static uint32_t GetHashCode(const T& p_Key)
    {
        if constexpr (requires { p_Key.GetHashCode(); })
        {
            return p_Key.GetHashCode();
        }
        else
        {
            return hash::Fnv1a(reinterpret_cast<const char*>(&p_Key), sizeof(T));
        }
    }
};

template<typename TKey, typename TValue> class THashMapIterator : public TIterator<TPair<TKey, TValue>>
{
  public:
    using NodeType = THashMapNode<TKey, TValue>;

    THashMapIterator(const SHashMapInfo<TKey, TValue>* p_MapInfo, uint32_t p_Bucket, NodeType* p_CurrentNode)
        : TIterator<TPair<TKey, TValue>>(&p_CurrentNode->m_data), m_info(p_MapInfo), m_nBucket(p_Bucket)
    {}

    THashMapIterator(const SHashMapInfo<TKey, TValue>* p_MapInfo) : TIterator<TPair<TKey, TValue>>(nullptr), m_info(p_MapInfo), m_nBucket(UINT32_MAX)
    {}

    const NodeType& Node() const
    {
        return *reinterpret_cast<const NodeType*>(reinterpret_cast<const char*>(this->m_pCurrent) - offsetof(NodeType, m_data));
    }

    NodeType& Node()
    {
        return const_cast<NodeType&>(static_cast<const THashMapIterator&>(*this).Node());
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
    const SHashMapInfo<TKey, TValue>* m_info = nullptr;
    uint32_t m_nBucket = 0;
};

template<typename TKey, typename TValue, typename THashMapPolicy = TDefaultHashMapPolicy<TKey>> class THashMap
{
  public:
    using NodeType = THashMapNode<const TKey, TValue>;
    using Iterator = THashMapIterator<const TKey, TValue>;
    using ConstIterator = THashMapIterator<const TKey, const TValue>;
    using ConstMapInfo = const SHashMapInfo<const TKey, const TValue>*;

    THashMap()
    {
        Init(4);
    }

    explicit THashMap(uint32_t p_BucketCount)
    {
        Init(p_BucketCount);
    }

    ~THashMap()
    {
        Clear();

        IAllocator* allocator = (*Globals::MemoryManager)->m_pNormalAllocator;

        if (m_info.m_pBuckets)
        {
            allocator->Free(m_info.m_pBuckets);
        }

        if (m_info.m_pNodes)
        {
            allocator->Free(m_info.m_pNodes);
        }
    }

    THashMap(const THashMap& p_Other)
    {
        Init(p_Other.m_info.m_nBuckets);

        for (const auto& pair : p_Other)
        {
            Insert(pair.first, pair.second);
        }
    }

    THashMap& operator=(const THashMap& p_Other)
    {
        if (this == &p_Other)
        {
            return *this;
        }

        Clear();

        Init(p_Other.m_info.m_nBuckets);

        for (const auto& pair : p_Other)
        {
            Insert(pair.first, pair.second);
        }

        return *this;
    }

    THashMap(THashMap&& p_Other)
    {
        m_info = p_Other.m_info;
        m_nSize = p_Other.m_nSize;
        m_iFree = p_Other.m_iFree;

        p_Other.m_info.m_pBuckets = nullptr;
        p_Other.m_info.m_pNodes = nullptr;
        p_Other.m_info.m_nBuckets = 0;
        p_Other.m_nSize = 0;
        p_Other.m_iFree = UINT32_MAX;
    }

    THashMap& operator=(THashMap&& p_Other)
    {
        if (this == &p_Other)
        {
            return *this;
        }

        IAllocator* allocator = (*Globals::MemoryManager)->m_pNormalAllocator;

        if (m_info.m_pBuckets)
        {
            allocator->Free(m_info.m_pBuckets);
        }

        if (m_info.m_pNodes)
        {
            allocator->Free(m_info.m_pNodes);
        }

        m_info = p_Other.m_info;
        m_nSize = p_Other.m_nSize;
        m_iFree = p_Other.m_iFree;

        p_Other.m_info.m_pBuckets = nullptr;
        p_Other.m_info.m_pNodes = nullptr;
        p_Other.m_info.m_nBuckets = 0;
        p_Other.m_nSize = 0;
        p_Other.m_iFree = UINT32_MAX;

        return *this;
    }

    void Init(uint32_t p_BucketCount)
    {
        if (p_BucketCount < 4)
        {
            p_BucketCount = 4;
        }

        IAllocator* allocator = (*Globals::MemoryManager)->m_pNormalAllocator;

        m_info.m_nBuckets = p_BucketCount;
        m_info.m_pBuckets = static_cast<uint32_t*>(allocator->Allocate(sizeof(uint32_t) * p_BucketCount));
        m_info.m_pNodes = static_cast<NodeType*>(allocator->AllocateAligned(sizeof(NodeType) * p_BucketCount, alignof(NodeType)));

        std::fill_n(m_info.m_pBuckets, p_BucketCount, UINT32_MAX);

        m_nSize = 0;
        m_iFree = UINT32_MAX;
    }

    void EnsureCapacity(uint32_t p_ElementCount)
    {
        if (m_info.m_nBuckets < p_ElementCount)
        {
            uint32_t newBucketCount = static_cast<uint32_t>(m_info.m_nBuckets * 1.5f);

            if (newBucketCount == 0)
            {
                newBucketCount = 4;
            }

            Rehash(newBucketCount);
        }
    }

    ConstIterator Begin() const
    {
        if (m_info.m_nBuckets == 0)
        {
            return ConstIterator(reinterpret_cast<ConstMapInfo>(&m_info));
        }

        for (uint32_t i = 0; i < m_info.m_nBuckets; ++i)
        {
            const uint32_t nodeIndex = m_info.m_pBuckets[i];

            if (nodeIndex != UINT32_MAX)
            {
                return ConstIterator(reinterpret_cast<ConstMapInfo>(&m_info), i, reinterpret_cast<NodeType*>(&m_info.m_pNodes[nodeIndex]));
            }
        }

        return ConstIterator(reinterpret_cast<ConstMapInfo>(&m_info));
    }

    Iterator Begin()
    {
        if (m_info.m_nBuckets == 0)
        {
            return Iterator(&m_info);
        }

        for (uint32_t i = 0; i < m_info.m_nBuckets; ++i)
        {
            const uint32_t nodeIndex = m_info.m_pBuckets[i];

            if (nodeIndex != UINT32_MAX)
            {
                return Iterator(&m_info, i, &m_info.m_pNodes[nodeIndex]);
            }
        }

        return Iterator(&m_info);
    }

    ConstIterator End() const
    {
        return ConstIterator(reinterpret_cast<ConstMapInfo>(&m_info));
    }

    Iterator End()
    {
        return Iterator(&m_info);
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

    size_t Size() const
    {
        return m_nSize;
    }

    bool IsEmpty() const
    {
        return m_nSize == 0;
    }

    const TValue& At(const TKey& p_Key) const
    {
        ConstIterator iterator = Find(p_Key);

        if (iterator != End())
        {
            return iterator->second;
        }

        throw std::out_of_range("Key not found in the hash map!");
    }

    TValue& at(const TKey& p_Key)
    {
        Iterator iterator = Find(p_Key);

        if (iterator != End())
        {
            return iterator->second;
        }

        throw std::out_of_range("Key not found in the hash map!");
    }

    TValue& operator[](const TKey& p_Key)
    {
        Iterator iterator = Find(p_Key);

        if (iterator != End())
        {
            return iterator->second;
        }

        return insert(p_Key, TValue{})->second;
    }

    bool Contains(const TKey& p_Key) const
    {
        return Find(p_Key) != End();
    }

    bool Contains(const TKey& p_Key)
    {
        return Find(p_Key) != End();
    }

    ConstIterator Find(const TKey& p_Key) const
    {
        if (auto* node = FindNode(p_Key))
        {
            return ConstIterator(
                reinterpret_cast<ConstMapInfo>(&m_info), THashMapPolicy::GetHashCode(p_Key) % m_info.m_nBuckets, reinterpret_cast<NodeType*>(node)
            );
        }

        return End();
    }

    Iterator Find(const TKey& p_Key)
    {
        if (auto* node = FindNode(p_Key))
        {
            return Iterator(&m_info, THashMapPolicy::GetHashCode(p_Key) % m_info.m_nBuckets, node);
        }

        return End();
    }

    Iterator Insert(const TKey& p_Key, const TValue& p_Value)
    {
        if (!m_info.m_pBuckets)
        {
            return Iterator(&m_info);
        }

        EnsureCapacity(m_nSize + 1);

        const uint32_t hash = THashMapPolicy::GetHashCode(p_Key);
        const uint32_t bucketIndex = static_cast<uint32_t>(hash % m_info.m_nBuckets);
        uint32_t nodeIndex = m_info.m_pBuckets[bucketIndex];

        while (nodeIndex != UINT32_MAX)
        {
            NodeType& node = m_info.m_pNodes[nodeIndex];

            if (node.m_data.first == p_Key)
            {
                return Iterator(&m_info, bucketIndex, &node);
            }

            nodeIndex = node.m_iNext;
        }

        uint32_t newNodeIndex;

        if (m_iFree == UINT32_MAX)
        {
            newNodeIndex = m_nSize++;
        }
        else
        {
            newNodeIndex = m_iFree;
            m_iFree = m_info.m_pNodes[newNodeIndex].m_iNext;
        }

        NodeType& newNode = m_info.m_pNodes[newNodeIndex];
        newNode.m_data = TPair<TKey, TValue>(p_Key, p_Value);
        newNode.m_iNext = m_info.m_pBuckets[bucketIndex];

        m_info.m_pBuckets[bucketIndex] = newNodeIndex;

        return Iterator(&m_info, bucketIndex, &newNode);
    }

    Iterator InsertOrAssign(const TKey& p_Key, const TValue& p_Value)
    {
        if (!m_info.m_pBuckets)
        {
            return Iterator(&m_info);
        }

        EnsureCapacity(m_nSize + 1);

        const uint32_t hash = THashMapPolicy::GetHashCode(p_Key);
        const uint32_t bucketIndex = static_cast<uint32_t>(hash % m_info.m_nBuckets);
        uint32_t nodeIndex = m_info.m_pBuckets[bucketIndex];

        while (nodeIndex != UINT32_MAX)
        {
            NodeType& node = m_info.m_pNodes[nodeIndex];

            if (node.m_data.first == p_Key)
            {
                node.m_data.second = p_Value;

                return Iterator(&m_info, bucketIndex, &node);
            }

            nodeIndex = node.m_iNext;
        }

        uint32_t newNodeIndex;

        if (m_iFree == UINT32_MAX)
        {
            newNodeIndex = m_nSize++;
        }
        else
        {
            newNodeIndex = m_iFree;
            m_iFree = m_info.m_pNodes[newNodeIndex].m_iNext;
        }

        NodeType& newNode = m_info.m_pNodes[newNodeIndex];
        newNode.m_data = TPair<TKey, TValue>(p_Key, p_Value);
        newNode.m_iNext = m_info.m_pBuckets[bucketIndex];

        m_info.m_pBuckets[bucketIndex] = newNodeIndex;

        return Iterator(&m_info, bucketIndex, &newNode);
    }

    void Rehash(uint32_t p_NewBucketCount)
    {
        if (p_NewBucketCount < 4)
        {
            p_NewBucketCount = 4;
        }

        IAllocator* allocator = (*Globals::MemoryManager)->m_pNormalAllocator;
        uint32_t* newBuckets = static_cast<uint32_t*>(allocator->Allocate(sizeof(uint32_t) * p_NewBucketCount));
        NodeType* newNodes = static_cast<NodeType*>(allocator->AllocateAligned(sizeof(NodeType) * p_NewBucketCount, alignof(NodeType)));

        std::fill_n(newBuckets, p_NewBucketCount, UINT32_MAX);

        uint32_t newSize = 0;

        if (m_info.m_nBuckets > 0 && m_info.m_pBuckets && m_info.m_pNodes)
        {
            for (uint32_t i = 0; i < m_info.m_nBuckets; ++i)
            {
                uint32_t nodeIndex = m_info.m_pBuckets[i];

                while (nodeIndex != UINT32_MAX)
                {
                    NodeType& oldNode = m_info.m_pNodes[nodeIndex];
                    const uint32_t hash = THashMapPolicy::GetHashCode(oldNode.m_data.first);
                    const uint32_t newBucketIndex = static_cast<uint32_t>(hash % p_NewBucketCount);

                    NodeType& newNode = newNodes[newSize];
                    newNode.m_data = oldNode.m_data;
                    newNode.m_iNext = newBuckets[newBucketIndex];
                    newBuckets[newBucketIndex] = newSize++;

                    nodeIndex = oldNode.m_iNext;
                }
            }

            for (uint32_t i = 0; i < m_info.m_nBuckets; ++i)
            {
                uint32_t nodeIndex = m_info.m_pBuckets[i];

                while (nodeIndex != UINT32_MAX)
                {
                    NodeType& node = m_info.m_pNodes[nodeIndex];

                    node.m_data.second.~TValue();
                    node.m_data.first.~TKey();

                    nodeIndex = node.m_iNext;
                }
            }
        }

        if (m_info.m_pBuckets)
        {
            allocator->Free(m_info.m_pBuckets);
        }

        if (m_info.m_pNodes)
        {
            allocator->Free(m_info.m_pNodes);
        }

        m_info.m_nBuckets = p_NewBucketCount;
        m_info.m_pBuckets = newBuckets;
        m_info.m_pNodes = newNodes;
        m_nSize = newSize;
        m_iFree = UINT32_MAX;
    }

    bool Erase(const TKey& p_Key)
    {
        if (!m_info.m_pBuckets)
        {
            return false;
        }

        const uint32_t hash = THashMapPolicy::GetHashCode(p_Key);
        const uint32_t bucketIndex = static_cast<uint32_t>(hash % m_info.m_nBuckets);
        uint32_t nodeIndex = m_info.m_pBuckets[bucketIndex];

        while (nodeIndex != UINT32_MAX)
        {
            NodeType* currentNode = &m_info.m_pNodes[nodeIndex];

            if (currentNode->m_data.first == p_Key)
            {
                EraseNode(bucketIndex, currentNode);
                return true;
            }

            nodeIndex = currentNode->m_iNext;
        }

        return false;
    }

    Iterator Erase(Iterator p_Where)
    {
        if (!m_info.m_pBuckets || !p_Where.m_pCurrent)
        {
            return End();
        }

        Iterator next = p_Where;
        ++next;

        const TKey& key = p_Where->first;
        const uint32_t hash = THashMapPolicy::GetHashCode(key);
        const uint32_t bucketIndex = static_cast<uint32_t>(hash % m_info.m_nBuckets);

        EraseNode(bucketIndex, p_Where.m_pCurrent);

        return next;
    }

    void Clear()
    {
        if (!m_info.m_pBuckets || !m_info.m_pNodes)
        {
            return;
        }

        for (uint32_t i = 0; i < m_info.m_nBuckets; ++i)
        {
            uint32_t nodeIndex = m_info.m_pBuckets[i];

            while (nodeIndex != UINT32_MAX)
            {
                NodeType& node = m_info.m_pNodes[nodeIndex];

                node.m_data.second.~TValue();
                node.m_data.first.~TKey();

                nodeIndex = node.m_iNext;
            }
        }

        std::fill_n(m_info.m_pBuckets, m_info.m_nBuckets, UINT32_MAX);

        for (uint32_t i = 0; i < m_info.m_nBuckets; ++i)
        {
            m_info.m_pNodes[i].m_iNext = UINT32_MAX;
        }

        m_nSize = 0;
        m_iFree = UINT32_MAX;
    }

  private:
    NodeType* FindNode(const TKey& p_Key) const
    {
        if (!m_info.m_pBuckets)
        {
            return nullptr;
        }

        const uint32_t hash = THashMapPolicy::GetHashCode(p_Key);
        const uint32_t bucketIndex = static_cast<uint32_t>(hash % m_info.m_nBuckets);
        uint32_t nodeIndex = m_info.m_pBuckets[bucketIndex];

        while (nodeIndex != UINT32_MAX)
        {
            NodeType& node = m_info.m_pNodes[nodeIndex];

            if (node.m_data.m_key == p_Key)
            {
                return &node;
            }

            nodeIndex = node.m_iNext;
        }

        return nullptr;
    }

    void EraseNode(uint32_t p_BucketIndex, NodeType* p_TargetNode)
    {
        if (!m_info.m_pBuckets || !p_TargetNode)
        {
            return;
        }

        uint32_t* bucket = &m_info.m_pBuckets[p_BucketIndex];
        uint32_t nodeIndex = *bucket;
        NodeType* previousNode = nullptr;

        while (nodeIndex != UINT32_MAX)
        {
            NodeType* currentNode = &m_info.m_pNodes[nodeIndex];

            if (currentNode == p_TargetNode)
            {
                if (previousNode)
                {
                    previousNode->m_iNext = currentNode->m_iNext;
                }
                else
                {
                    *bucket = currentNode->m_iNext;
                }

                currentNode->m_data.second.~TValue();
                currentNode->m_data.first.~TKey();

                currentNode->m_iNext = m_iFree;
                m_iFree = nodeIndex;
                --m_nSize;

                return;
            }

            previousNode = currentNode;
            nodeIndex = currentNode->m_iNext;
        }
    }

    uint32_t m_nSize = 0;
    int32_t m_iFree = UINT32_MAX;
    SHashMapInfo<const TKey, TValue> m_info;
};
