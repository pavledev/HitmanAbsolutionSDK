#pragma once

#include "THashMapNode.h"

template <typename TKey, typename TValue>
struct SHashMapInfo
{
    unsigned int m_nBuckets;
    int* m_pBuckets;
    THashMapNode<TKey, TValue>* m_pNodes;
};
