#pragma once

#include "ZCameraEntity.h"

class ZHM5MainCamera : public ZCameraEntity
{
public:
	virtual ~ZHM5MainCamera() = 0;
	virtual ZVariantRef GetVariantRef() const = 0;
	virtual int AddRef() = 0;
	virtual int Release() = 0;
	virtual void* QueryInterface(STypeID* iid) = 0;
	virtual void Init() = 0;
	virtual void Start() = 0;
	virtual void SetFovYDeg(float fFovYDeg) = 0;

	virtual float4 GetListenerPos() const = 0;

	void SetCameraDirection(const float4& vLookTo);

private:
	PAD(0x3C0);
};

static_assert(sizeof(ZHM5MainCamera) == 0x550);
static_assert(alignof(ZHM5MainCamera) == 0x10);
