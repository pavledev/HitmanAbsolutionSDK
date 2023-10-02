#pragma once

#include "ZObjectRef.h"

class ZVariantRef : public ZObjectRef
{
public:
	ZVariantRef(void* pObject, STypeID* const typeID);
};
