#pragma once

#include "../ZString.h"

#include <Hash.h>

struct STypeID;

template <typename TKey, typename TValue>
class TDefaultHashMapPolicy
{
};

template <>
struct TDefaultHashMapPolicy<ZString, STypeID*>
{
	static unsigned int GetHashCode(const ZString& key)
	{
		return Hash::Fnv1a(key.ToCString(), key.Length());
	}
};
