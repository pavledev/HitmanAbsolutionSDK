#pragma once

#include "ZVariantRef.h"

class IComponentInterface
{
public:
	virtual ~IComponentInterface() = 0;
	virtual ZVariantRef GetVariantRef() const = 0;
	virtual int AddRef() = 0;
	virtual int Release() = 0;
	virtual void* QueryInterface(STypeID* typeID) = 0;
};
