#pragma once

#include "Glacier/IComponentInterface.h"
#include "ZRenderDevice.h"

class alignas(8) ZRenderManager : public IComponentInterface
{
public:
	ZRenderDevice* GetRenderDevice() const;

private:
	PAD(0x8C);
	ZRenderDevice* m_pRenderDevice; //0x90
	PAD(0x1B4);
};

static_assert(sizeof(ZRenderManager) == 0x248);
static_assert(alignof(ZRenderManager) == 0x8);
