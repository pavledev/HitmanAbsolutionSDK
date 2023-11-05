#pragma once

#include "../ZString.h"
#include "../Entity/ZPropertyRegistry.h"

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

template <>
struct TDefaultHashMapPolicy<unsigned int, ZPropertyRegistry::SPropertyName>
{
	static unsigned int GetHashCode(const unsigned int& key)
	{
		const char* data = reinterpret_cast<const char*>(&key);
		const size_t length = sizeof(unsigned int);

		return Hash::Fnv1a(data, length);
	}
};
