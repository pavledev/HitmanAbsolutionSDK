#pragma once

#include <DirectXMath.h>

#include <directxtk/SimpleMath.h>

#include "float4.h"
#include "SMatrix43.h"
#include "SMatrix44.h"

struct SMatrix
{
	SMatrix() :
		XAxis(1.f, 0.f, 0.f, 0.f),
		YAxis(0.f, 1.f, 0.f, 0.f),
		ZAxis(0.f, 0.f, 1.f, 0.f),
		Trans(0.f, 0.f, 0.f, 1.f)
	{
	}

	SMatrix(const SMatrix43& other) :
		XAxis(other.XAxis.x, other.XAxis.y, other.XAxis.z, 0.f),
		YAxis(other.YAxis.x, other.YAxis.y, other.YAxis.z, 0.f),
		ZAxis(other.ZAxis.x, other.ZAxis.y, other.ZAxis.z, 0.f),
		Trans(other.Trans.x, other.Trans.y, other.Trans.z, 1.f)
	{
	}

	SMatrix(const SMatrix44& other) :
		XAxis(other.m11, other.m12, other.m13, other.m14),
		YAxis(other.m21, other.m22, other.m23, other.m24),
		ZAxis(other.m31, other.m32, other.m33, other.m34),
		Trans(other.m41, other.m42, other.m43, other.m44)
	{
	}

	SMatrix(const DirectX::XMMATRIX& other) :
		XAxis(other.r[0]),
		YAxis(other.r[1]),
		ZAxis(other.r[2]),
		Trans(other.r[3])
	{
	}

	SMatrix(const DirectX::SimpleMath::Matrix& other) :
		XAxis(other._11, other._12, other._13, other._14),
		YAxis(other._21, other._22, other._23, other._24),
		ZAxis(other._31, other._32, other._33, other._34),
		Trans(other._41, other._42, other._43, other._44)
	{
	}

	SMatrix(float m11, float m12, float m13, float m14,
		float m21, float m22, float m23, float m24,
		float m31, float m32, float m33, float m34,
		float m41, float m42, float m43, float m44)
	{
		this->m11 = m11;
		this->m12 = m12;
		this->m13 = m13;
		this->m14 = m14;
		this->m21 = m21;
		this->m22 = m22;
		this->m23 = m23;
		this->m24 = m24;
		this->m31 = m31;
		this->m32 = m32;
		this->m33 = m33;
		this->m34 = m34;
		this->m41 = m41;
		this->m42 = m42;
		this->m43 = m43;
		this->m44 = m44;
	}

	DirectX::XMMATRIX& DX()
	{
		return *reinterpret_cast<DirectX::XMMATRIX*>(this);
	}

	const DirectX::XMMATRIX& DX() const
	{
		return *reinterpret_cast<const DirectX::XMMATRIX*>(this);
	}

	float4 operator*(const float4& other) const
	{
		return DirectX::XMVector4Transform(other.m, DX());
	}

	SMatrix operator*(const SMatrix& other) const
	{
		return DirectX::XMMatrixMultiply(DX(), other.DX());
	}

	SMatrix Inverse() const
	{
		return DirectX::XMMatrixInverse(nullptr, DX());
	}

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

		struct
		{
			float4 Left;
			float4 Backward;
			float4 Up;
			float4 Pos;
		};

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
	};
};
