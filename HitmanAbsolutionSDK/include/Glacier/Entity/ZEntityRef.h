#pragma once

#include "ZEntityType.h"
#include "../TypeInfo/ZTypeRegistry.h"

#include <Common.h>
#include <Reflection.h>
#include <Global.h>

class ZEntityImpl;
class ZVariantRef;

class HitmanAbsolutionSDK_API ZEntityRef
{
public:
	ZEntityRef() = default;
	ZEntityRef(ZEntityType** entityTypePtrPtr);
	bool operator==(const ZEntityRef& rhs) const;
	bool operator!=(const ZEntityRef& rhs) const;
	ZEntityImpl* GetEntityImpl() const;
	ZEntityType** GetEntityTypePtrPtr() const;
	void* QueryInterfacePtr(const STypeID* interfaceID) const;
	//ZEntityRef GetLogicalParent() const;

	template <typename T>
	T* QueryInterfacePtr() const
	{
		if (!*TypeRegistry)
		{
			return nullptr;
		}

		const STypeID* typeID = (*TypeRegistry)->GetType(GlacierTypeName<T>);

		if (typeID)
		{
			return static_cast<T*>(QueryInterfacePtr(typeID));
		}

		return nullptr;
	}

private:
	ZEntityType** m_pEntityTypePtrPtr;
};
