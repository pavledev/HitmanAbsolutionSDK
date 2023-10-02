#include "Glacier/Camera/ZCameraEntity.h"

#include <Function.h>
#include <Global.h>

SMatrix ZCameraEntity::GetViewMatrix() const
{
	return GetObjectToWorldMatrix().Inverse();
}

void ZCameraEntity::SetNearZ(float fNearZ)
{
	Function::CallMethod<ZCameraEntity*, float>(BaseAddress + 0x5645B0, this, fNearZ);
}

void ZCameraEntity::SetFarZ(float fFarZ)
{
	Function::CallMethod<ZCameraEntity*, float>(BaseAddress + 0xA0BF0, this, fFarZ);
}
