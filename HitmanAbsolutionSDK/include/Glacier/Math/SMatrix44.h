#pragma once

#include "SVector4.h"

struct SMatrix44
{
	SMatrix44()
	{
		r[0] = SVector4(1.f, 0.f, 0.f, 0.f);
		r[1] = SVector4(0.f, 1.f, 0.f, 0.f);
		r[2] = SVector4(0.f, 0.f, 1.f, 0.f);
		r[3] = SVector4(0.f, 0.f, 0.f, 1.f);
	}

	union
	{
		struct
		{
			float m11;
			float m12;
			float m13;
			float m14;
			float m21;
			float m22;
			float m23;
			float m24;
			float m31;
			float m32;
			float m33;
			float m34;
			float m41;
			float m42;
			float m43;
			float m44;
		};

		SVector4 r[4];
		float v[16];
	};
};
