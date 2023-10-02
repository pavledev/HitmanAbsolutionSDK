#pragma once

#include "../ZMutex.h"
#include "../Templates/THashMap.h"
#include "Hash.h"
#include "../ZString.h"

struct STypeID;
class IType;
class ITypeConverter;
class ZVariantRef;
struct SComponentMapEntry;

class HitmanAbsolutionSDK_API ZTypeRegistry
{
public:
	virtual ~ZTypeRegistry() = default;

	const STypeID* GetType(const ZString& typeName);

private:
	class ZTypeMapHashPolicy
	{
	public:
		static unsigned int GetHashCode(const ZString& key)
		{
			return Hash::Fnv1aLower(key.ToCString(), key.Length());
		}
	};

	PAD(0xC);
	ZMutex m_mutex;
	THashMap<ZString, STypeID*, ZTypeMapHashPolicy> m_typeNameMap;
};
