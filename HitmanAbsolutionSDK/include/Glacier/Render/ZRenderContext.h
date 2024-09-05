#pragma once

#include "ZRenderGBuffer.h"

class ZRenderContext
{
public:
	PAD(0x40);
	ZRenderDevice* m_pRenderDevice; //0x40
	PAD(0x2C);
	ZRenderGBuffer* m_pGBuffer; //0x70
};
