#include <DirectXMath.h>

#include "Glacier/Math/SMatrix.h"
#include "Glacier/Math/SMatrix43.h"
#include "Glacier/Math/SMatrix44.h"

SMatrix::SMatrix() :
	XAxis(1.f, 0.f, 0.f, 0.f),
	YAxis(0.f, 1.f, 0.f, 0.f),
	ZAxis(0.f, 0.f, 1.f, 0.f),
	Trans(0.f, 0.f, 0.f, 1.f)
{

}

SMatrix::SMatrix(const SMatrix43& other) :
	XAxis(other.XAxis.x, other.XAxis.y, other.XAxis.z, 0.f),
	YAxis(other.YAxis.x, other.YAxis.y, other.YAxis.z, 0.f),
	ZAxis(other.ZAxis.x, other.ZAxis.y, other.ZAxis.z, 0.f),
	Trans(other.Trans.x, other.Trans.y, other.Trans.z, 1.f)
{

}

SMatrix::SMatrix(const SMatrix44& other) :
	XAxis(other.m11, other.m12, other.m13, other.m14),
	YAxis(other.m21, other.m22, other.m23, other.m24),
	ZAxis(other.m31, other.m32, other.m33, other.m34),
	Trans(other.m41, other.m42, other.m43, other.m44)
{

}

SMatrix::SMatrix(const DirectX::XMMATRIX& other) :
	XAxis(other.r[0]),
	YAxis(other.r[1]),
	ZAxis(other.r[2]),
	Trans(other.r[3])
{

}

SMatrix::operator DirectX::XMMATRIX() const
{
	return DirectX::XMMATRIX(XAxis.m, YAxis.m, ZAxis.m, Trans.m);
}

SMatrix SMatrix::Inverse() const
{
	return DirectX::XMMatrixInverse(nullptr, DirectX::XMMATRIX(*this));
}
