#pragma once

#include <d3d11.h>

#include "../Templates/TRenderResourceImpl.h"
#include "SRenderTexture2DDesc.h"
#include "IRenderResource.h"

#include <Common.h>

class ZRenderTexture2D : public TRenderResourceImpl<IRenderResource, 1>
{
public:
	virtual ~ZRenderTexture2D() = 0;
	virtual void Allocate(unsigned int nLocation) = 0;
	virtual void Free() = 0;

	const SRenderTexture2DDesc& GetDesc() const;
	ID3D11Texture2D* GetTexture2D() const;

private:
	SRenderTexture2DDesc m_Description;
	ID3D11Texture2D* m_pTexture2D;
	PAD(0x14);
};

static_assert(sizeof(ZRenderTexture2D) == 0x60);
static_assert(alignof(ZRenderTexture2D) == 0x8);
