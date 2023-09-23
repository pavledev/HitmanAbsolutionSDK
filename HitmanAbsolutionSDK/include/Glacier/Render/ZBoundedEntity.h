#pragma once

#include "ZSpatialEntity.h"

class ZBoundedEntity : public ZSpatialEntity
{
public:
	virtual ~ZBoundedEntity() = 0;
	virtual ZVariantRef GetVariantRef() const = 0;
	virtual int AddRef() = 0;
	virtual int Release() = 0;
	virtual void* QueryInterface(STypeID* iid) = 0;
	virtual void Init() = 0;
	virtual const ZBoundedEntity* QueryBoundedEntity() const = 0;
	virtual ZBoundedEntity* QueryBoundedEntity() = 0;
	virtual float4 GetLocalCenter() const = 0;
	virtual float4 GetLocalHalfSize() const = 0;
	virtual void UpdateBounds() = 0;

private:
	PAD(0x18);
};

static_assert(sizeof(ZBoundedEntity) == 0x58);
static_assert(alignof(ZBoundedEntity) == 0x4);
