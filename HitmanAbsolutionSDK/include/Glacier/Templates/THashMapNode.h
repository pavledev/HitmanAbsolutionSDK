#pragma once

#include "TPair.h"

template <typename TKey, typename TValue>
struct THashMapNode
{
	int m_iNext;
	TPair<TKey, TValue> m_data;
};
