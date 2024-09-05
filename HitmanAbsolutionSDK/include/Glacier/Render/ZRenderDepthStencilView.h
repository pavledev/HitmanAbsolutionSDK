#pragma once

#include <d3d11.h>

#include "SRenderDepthStencilViewDesc.h"
#include "ZRenderTexture2D.h"

class ZRenderDepthStencilView
{
public:
	virtual ~ZRenderDepthStencilView() = default;

	SRenderDepthStencilViewDesc m_Description;
	ZRenderTexture2D* m_pTexture;
	ID3D11DepthStencilView* m_pDSV;
};
