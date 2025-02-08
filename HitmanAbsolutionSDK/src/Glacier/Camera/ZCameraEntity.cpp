#include "Glacier/Camera/ZCameraEntity.h"

#include <Function.h>
#include <Global.h>

void ZCameraEntity::UpdateProjection()
{
	Function::CallMethod<ZCameraEntity*>(BaseAddress + 0x5BDD60, this);
}

SMatrix ZCameraEntity::GetViewMatrix() const
{
	return GetObjectToWorldMatrix().Inverse();
}

void ZCameraEntity::SetFovYDeg(const float fovYDeg)
{
	m_fFovYDeg = fovYDeg;
}

void ZCameraEntity::SetNearZ(float fNearZ)
{
	Function::CallMethod<ZCameraEntity*, float>(BaseAddress + 0x5645B0, this, fNearZ);
}

void ZCameraEntity::SetFarZ(float fFarZ)
{
	Function::CallMethod<ZCameraEntity*, float>(BaseAddress + 0xA0BF0, this, fFarZ);
}

TEntityRef<IRenderPostfilterControllerEntity>& ZCameraEntity::GetRenderPostfilterControllerEntity()
{
	return m_rPostfilter;
}

void ZCameraEntity::SetFovY(const float fovY)
{
	m_fFovY = fovY;
}
