#include "Glacier/Math/SMatrix33.h"

SMatrix33::SMatrix33() :
	XAxis(1.f, 0.f, 0.f),
	YAxis(0.f, 1.f, 0.f),
	ZAxis(0.f, 0.f, 1.f)
{

}

SMatrix33::SMatrix33(SVector3 xAxis, SVector3 yAxis, SVector3 zAxis) : XAxis(xAxis), YAxis(yAxis), ZAxis(zAxis)
{

}
