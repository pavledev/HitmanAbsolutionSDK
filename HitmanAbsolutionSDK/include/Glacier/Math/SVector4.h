#pragma once

#include "SVector3.h"

struct SVector4
{
	SVector4() : x(0.f), y(0.f), z(0.f), w(0.f)
	{
	}

	SVector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w)
	{
	}

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
