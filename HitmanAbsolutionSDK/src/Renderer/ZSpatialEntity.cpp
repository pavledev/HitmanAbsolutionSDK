#include "Glacier/Render/ZSpatialEntity.h"
#include "Function.h"
#include "Global.h"

SMatrix ZSpatialEntity::GetObjectToWorldMatrix() const
{
	return Function::CallRVOMethodAndReturn<SMatrix, const ZSpatialEntity*>(BaseAddress + 0x2A7C40, this);
}

void ZSpatialEntity::SetObjectToWorldMatrix(const SMatrix& mObjectToWorld)
{
	Function::CallMethod<ZSpatialEntity*, const SMatrix&>(BaseAddress + 0x460D00, this, mObjectToWorld);
}

float4 ZSpatialEntity::GetWorldPosition() const
{
	return Function::CallRVOMethodAndReturn<float4, const ZSpatialEntity*>(BaseAddress + 0x379820, this);
}

void ZSpatialEntity::SetWorldPosition(const float4& vWorldPosition)
{
	Function::CallMethod<ZSpatialEntity*, const float4&>(BaseAddress + 0x562060, this, vWorldPosition);
}
