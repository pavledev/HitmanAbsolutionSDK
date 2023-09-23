#include "Glacier/Math/SMatrix44.h"

SMatrix44::SMatrix44()
{
	r[0] = SVector4(1.f, 0.f, 0.f, 0.f);
	r[1] = SVector4(0.f, 1.f, 0.f, 0.f);
	r[2] = SVector4(0.f, 0.f, 1.f, 0.f);
	r[3] = SVector4(0.f, 0.f, 0.f, 1.f);
}
