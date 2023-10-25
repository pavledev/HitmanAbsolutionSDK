#pragma once

#include "ZObjectRef.h"
#include "../TypeInfo/ZTypeRegistry.h"

#include <Global.h>
#include <Reflection.h>

class ZVariantRef;

class HitmanAbsolutionSDK_API ZVariant : public ZObjectRef
{
public:
	ZVariant();
	ZVariant(STypeID* type);
	ZVariant(STypeID* type, const void* pData);
	ZVariant(const ZVariantRef& rhs);
	ZVariant(const ZVariant& rhs);
	void Set(const ZVariantRef& rhs);
	void Set(const ZVariant& rhs);
	void Set(STypeID* type);
	void Set(STypeID* type, const void* pData);
	void Allocate(STypeID* type);
	void Clear();

	template <typename T>
	T& Get()
	{
		return *static_cast<T*>(m_pData);
	}

	template <typename T>
	void Set(const T& data)
	{
		STypeID* typeID = (*TypeRegistry)->GetType(GlacierTypeName<T>);

		Set(typeID, static_cast<const void*>(&data));
	}
};
