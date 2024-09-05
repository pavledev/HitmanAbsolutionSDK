#pragma once

#include "ERenderFormat.h"

struct SRenderDepthStencilViewDesc
{
	enum EViewDimension
	{
		VIEW_DIMENSION_TEXTURE2D = 1,
		VIEW_DIMENSION_TEXTURE2DMS = 2
	};

	struct SViewTexture2D
	{
		unsigned int nMipSlice;
	};

	ERenderFormat eFormat;
	EViewDimension eViewDimension;
	SViewTexture2D viewTexture2D;
};
