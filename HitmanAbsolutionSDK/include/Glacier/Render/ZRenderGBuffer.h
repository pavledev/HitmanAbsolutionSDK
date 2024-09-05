#pragma once

#include "Common.h"

#include "ZRenderDepthStencilView.h"

class ZRenderDevice;

class ZRenderGBuffer
{
public:
	PAD(0x10);
	ZRenderDevice* m_pRenderDevice; //0x10
	PAD(0x8);
	ZRenderDepthStencilView* m_pDepthDSV; //0x1C
};
