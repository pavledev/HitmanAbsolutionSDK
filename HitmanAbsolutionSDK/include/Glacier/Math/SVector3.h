#pragma once

#include <cmath>
#include <DirectXMath.h>

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

	SVector3(const DirectX::XMVECTOR& vector)
	{
		DirectX::XMStoreFloat3(reinterpret_cast<DirectX::XMFLOAT3*>(this), vector);
	}

	SVector3(const DirectX::XMFLOAT3& vector) : x(vector.x), y(vector.y), z(vector.z)
	{
	}

	SVector3 operator+(const SVector3& other)
	{
		SVector3 result;

		result.x = x + other.x;
		result.y = y + other.y;
		result.z = z + other.z;

		return result;
	}

	SVector3 operator-(const SVector3& other)
	{
		SVector3 result;

		result.x = x - other.x;
		result.y = y - other.y;
		result.z = z - other.z;

		return result;
	}

	SVector3 operator*(const float value) const
	{
		return SVector3(x * value, y * value, z * value);
	}

	const bool operator==(const SVector3& other) const
	{
		return x == other.x && y == other.y && z == other.z;
	}

	const bool operator!=(const SVector3& other) const
	{
		return !(x == other.x && y == other.y && z == other.z);
	}

	SVector3 operator/(const SVector3& other) const
	{
		return SVector3(x / other.x, y / other.y, z / other.z);
	}

	SVector3 operator/(const float other) const
	{
		return SVector3(x / other, y / other, z / other);
	}

	float Length() const
	{
		return std::sqrt(x * x + y * y + z * z);
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
