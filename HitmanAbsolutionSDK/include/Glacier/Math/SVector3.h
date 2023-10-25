#pragma once

#include "float4.h"

struct SVector3
{
	SVector3() : x(0.f), y(0.f), z(0.f)
	{
	}

	SVector3(float x, float y, float z) : x(x), y(y), z(z)
	{
	}

	SVector3(const float4& other) : x(other.x), y(other.y), z(other.z)
	{
	}

	union
	{
		struct
		{
			float x;
			float y;
			float z;
		};

		float v[3];
	};
};
