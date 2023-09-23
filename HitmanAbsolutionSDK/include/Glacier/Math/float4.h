#pragma once

#include <xmmintrin.h>

struct float4
{
	float4();
	float4(__m128 m);
	float4(float x, float y, float z, float w);

	union
	{
		__m128 m;

		struct
		{
			float x;
			float y;
			float z;
			float w;
		};
	};
};
