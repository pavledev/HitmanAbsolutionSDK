#include "Glacier/Math/SMatrix43.h"

SMatrix43::SMatrix43() :
	Rot(SVector3(1.f, 0.f, 0.f),
		SVector3(0.f, 1.f, 0.f),
		SVector3(0.f, 0.f, 1.f)),
	Trans(0.f, 0.f, 0.f)
{

}
