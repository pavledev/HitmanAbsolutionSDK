#pragma once

struct SVector3
{
	SVector3() : x(0.f), y(0.f), z(0.f)
	{
	}

	SVector3(float x, float y, float z) : x(x), y(y), z(z)
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
