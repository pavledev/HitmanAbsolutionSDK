#pragma once

#include "Entity/ZVariantRef.h"

class ZGenericMemberFunctionTarget;

class ZPinFunctor
{
public:
	void (*pfInvoke)(void (*param1)(), ZGenericMemberFunctionTarget* genericMemberFunctionTarget, const ZVariantRef& variantRef, unsigned int param4);
	void (*func)();
};
