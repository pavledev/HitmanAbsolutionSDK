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
