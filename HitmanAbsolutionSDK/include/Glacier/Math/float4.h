#pragma once

#include <xmmintrin.h>

struct float4
{
	float4()
	{
		m = _mm_setzero_ps();
	}

	float4(__m128 m)
	{
		this->m = m;
	}

	float4(float x, float y, float z, float w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	float4 operator-(const float4& p_Vec) const
	{
		return _mm_sub_ps(m, p_Vec.m);
	}

	float4 operator+(const float4& p_Vec) const
	{
		return _mm_add_ps(m, p_Vec.m);
	}

	float4 operator*(const float4& p_Vec) const
	{
		return _mm_mul_ps(m, p_Vec.m);
	}

	float4 operator*(float p_Value) const
	{
		return _mm_mul_ps(m, _mm_load1_ps(&p_Value));
	}

	float4 operator/(const float4& p_Vec) const
	{
		return _mm_div_ps(m, p_Vec.m);
	}

	float4 operator/(float p_Value) const
	{
		return _mm_div_ps(m, _mm_load1_ps(&p_Value));
	}

	float4& operator+=(const float4& p_Other)
	{
		m = _mm_add_ps(m, p_Other.m);

		return *this;
	}

	float4& operator-=(const float4& p_Other)
	{
		m = _mm_sub_ps(m, p_Other.m);

		return *this;
	}

	union
	{
		__m128 m;

		struct
		{
			float x;
			float y;
			float z;
			float w;
		};
	};
};
