#pragma once

#include <DirectXMath.h>

#include "float4.h"
#include "SMatrix43.h"
#include "SMatrix44.h"

struct SMatrix
{
	SMatrix();
	SMatrix(const SMatrix43& other);
	SMatrix(const SMatrix44& p_Other);
	SMatrix(const DirectX::XMMATRIX& other);
	SMatrix Inverse() const;
	explicit operator DirectX::XMMATRIX() const;

	union
	{
		float4 mat[4];
		float flt[4 * 4];

		struct
		{
			float4 XAxis;
			float4 YAxis;
			float4 ZAxis;
			float4 Trans;
		};
	};
};
