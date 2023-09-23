#include "Glacier/Math/float4.h"

float4::float4()
{
	m = _mm_setzero_ps();
}

float4::float4(__m128 m)
{
	this->m = m;
}

float4::float4(float x, float y, float z, float w)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}
