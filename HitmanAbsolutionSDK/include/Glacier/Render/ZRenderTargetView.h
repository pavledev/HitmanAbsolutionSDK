#pragma once

#include "ZRenderTexture2D.h"
#include "SRenderTargetViewDesc.h"

class ZRenderTargetView
{
public:
	virtual ~ZRenderTargetView() = 0;

	ZRenderTexture2D* GetTexture() const;
	void GetDesc(SRenderTargetViewDesc* pDescription);
	ID3D11RenderTargetView* GetRenderTargetView() const;

private:
	SRenderTargetViewDesc m_Description;
	ZRenderTexture2D* m_pTexture;
	ID3D11RenderTargetView* m_pRTV;
};

static_assert(sizeof(ZRenderTargetView) == 0x20);
static_assert(alignof(ZRenderTargetView) == 0x4);
