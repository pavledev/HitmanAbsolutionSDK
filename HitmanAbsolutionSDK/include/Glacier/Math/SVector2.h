#pragma once

struct SVector2
{
	SVector2() : x(0.f), y(0.f)
	{
	}

	SVector2(float x, float y) : x(x), y(y)
	{
	}

	union
	{
		struct
		{
			float x;
			float y;
		};

		float v[2];
	};
};
