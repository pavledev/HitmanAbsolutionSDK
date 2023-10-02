#pragma once

#include "Glacier/IComponentInterface.h"
#include "ZRenderDevice.h"
#include "../Templates/TEntityRef.h"
#include "IRenderDestinationEntity.h"

class HitmanAbsolutionSDK_API alignas(8) ZRenderManager : public IComponentInterface
{
public:
	ZRenderDevice* GetRenderDevice() const;
	TEntityRef<IRenderDestinationEntity> GetGameRenderDestinationEntity();
	TEntityRef<IRenderDestinationEntity> GetActiveRenderDestinationEntity() const;

private:
	PAD(0x8C);
	ZRenderDevice* m_pRenderDevice; //0x90
	PAD(0x1B4);
};

static_assert(sizeof(ZRenderManager) == 0x248);
static_assert(alignof(ZRenderManager) == 0x8);
