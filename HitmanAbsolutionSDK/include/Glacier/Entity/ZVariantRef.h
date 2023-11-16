#pragma once

#include "ZObjectRef.h"

class ZVariant;

class HitmanAbsolutionSDK_API ZVariantRef : public ZObjectRef
{
public:
	ZVariantRef();
	ZVariantRef(const ZVariant& rhs);
	ZVariantRef(void* pObject, STypeID* const typeID);
};
