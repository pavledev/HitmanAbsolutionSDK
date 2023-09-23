#pragma once

#include "SVector3.h"

struct SVector4
{
	SVector4();
	SVector4(float x, float y, float z, float w);

	union
	{
		struct
		{
			float x;
			float y;
			float z;
			float w;
		};

		struct
		{
			SVector3 n;
			float d;
		};

		float v[4];
	};
};
