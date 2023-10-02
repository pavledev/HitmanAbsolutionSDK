#pragma once

#include "SVector3.h"

struct SMatrix33
{
	SMatrix33() :
		XAxis(1.f, 0.f, 0.f),
		YAxis(0.f, 1.f, 0.f),
		ZAxis(0.f, 0.f, 1.f)
	{
	}

	SMatrix33(SVector3 xAxis, SVector3 yAxis, SVector3 zAxis) : XAxis(xAxis), YAxis(yAxis), ZAxis(zAxis)
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
			float m11;
			float m12;
			float m13;
			float m21;
			float m22;
			float m23;
			float m31;
			float m32;
			float m33;
		};

		float v[9];
		SVector3 r[3];
	};
};
