#pragma once

struct SVector3
{
	SVector3();
	SVector3(float x, float y, float z);

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
