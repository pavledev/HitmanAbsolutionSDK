#pragma once

#include "../ZString.h"

template <typename TKey, typename TValue> class TDefaultHashMapPolicy;
template <typename TKey, typename TValue, typename THashMapPolicy> class THashMap;

class HitmanAbsolutionSDK_API ZPropertyRegistry
{
public:
	static const ZString& GetPropertyName(const unsigned int propertyID);

private:
	struct SPropertyName
	{
		ZString m_sPropertyName;
		unsigned int m_nPropertyID;
	};

	inline static THashMap<unsigned int, SPropertyName, TDefaultHashMapPolicy<unsigned int, SPropertyName>>* s_PropertyNameMap;
};
