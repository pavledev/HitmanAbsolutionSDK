#pragma once

#include <dxgi.h>

#include "ZRenderTexture2D.h"
#include "ZRenderTargetView.h"
#include "SRenderDestinationDesc.h"

#include <Common.h>

class ZRenderSwapChain
{
public:
	virtual ~ZRenderSwapChain() = 0;
	virtual void SetDisableStereoscopic(bool bDisable) = 0;

	IDXGISwapChain* GetSwapChain() const;
	ZRenderTexture2D* GetBackBufferTexture() const;
	ZRenderTargetView* GetBackBufferRTV() const;

private:
	PAD(0x8);
	IDXGISwapChain* m_pSwapChain; //0xC
	PAD(0xC);
	ZRenderTexture2D* m_pBackBufferTexture; //0x1C
	ZRenderTargetView* m_pBackBufferRTV; //0x20
	PAD(0x10);
	SRenderDestinationDesc* m_pDescription; //0x34
	PAD(0x1C);
};

static_assert(sizeof(ZRenderSwapChain) == 0x54);
static_assert(alignof(ZRenderSwapChain) == 0x4);
