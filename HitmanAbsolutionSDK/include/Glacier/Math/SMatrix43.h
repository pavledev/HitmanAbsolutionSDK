#pragma once

#include "SVector3.h"
#include "SMatrix33.h"

struct SMatrix;

struct SMatrix43
{
	SMatrix43() :
		Rot(SVector3(1.f, 0.f, 0.f),
			SVector3(0.f, 1.f, 0.f),
			SVector3(0.f, 0.f, 1.f)),
		Trans(0.f, 0.f, 0.f)
	{
	}

	union
	{
		struct
		{
			SVector3 XAxis;
			SVector3 YAxis;
			SVector3 ZAxis;
		};

		struct
		{
			SMatrix33 Rot;
			SVector3 Trans;
		};

		struct
		{
			float m11;
			float m12;
			float m13;
			float m21;
			float m22;
			float m23;
			float m31;
			float m32;
			float m33;
			float m41;
			float m42;
			float m43;
		};

		float v[12];
		SVector3 r[4];
	};
};
