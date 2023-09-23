#pragma once

#include "ERenderFormat.h"

struct SRenderTargetViewDesc
{
	enum EViewDimension
	{
		VIEW_DIMENSION_TEXTURE2D = 1,
		VIEW_DIMENSION_TEXTURE2DMS = 2,
		VIEW_DIMENSION_TEXTURE2DARRAY = 3,
		VIEW_DIMENSION_TEXTURE3D = 4
	};

	struct alignas(4) SViewTexture2D
	{
		unsigned int nMipSlice;
	};

	struct alignas(4) SViewTexture2DArray
	{
		unsigned int nMipSlice;
		unsigned int nFirstArraySlice;
		unsigned int nArraySize;
	};

	struct alignas(4) SViewTexture3D //Size = 0xC
	{
		unsigned int nMipSlice;
		unsigned int nFirstWSlice;
		unsigned int nWSize;
	};

	ERenderFormat eFormat;
	EViewDimension eViewDimension;

	union
	{
		SViewTexture2D viewTexture2D;
		SViewTexture2DArray viewTexture2DArray;
		SViewTexture3D viewTexture3D;
	};
};
