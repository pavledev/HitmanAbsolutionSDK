#pragma once

struct SVector2
{
	SVector2();
	SVector2(float x, float y);

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
