#include "Glacier/Math/SVector4.h"

SVector4::SVector4()
{
	x = 0;
	y = 0;
	z = 0;
	w = 0;
}

SVector4::SVector4(float x, float y, float z, float w)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}
