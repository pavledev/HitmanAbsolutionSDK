#pragma once

#include "ZBoundedEntity.h"

class ZRenderableEntity : public ZBoundedEntity
{
public:
	virtual ~ZRenderableEntity() = 0;
	virtual ZVariantRef GetVariantRef() const = 0;
	virtual int AddRef() = 0;
	virtual int Release() = 0;
	virtual void* QueryInterface(STypeID* iid) = 0;
	virtual void Init() = 0;
	virtual void OnSpatialChange() = 0;
	virtual void OnVisibleOnScreenChanged(bool bVisibleOnScreen) = 0;

private:
	PAD(0x8);
};

static_assert(sizeof(ZRenderableEntity) == 0x60);
static_assert(alignof(ZRenderableEntity) == 0x4);
