#include "Glacier/Render/ZSpatialEntity.h"
#include "Function.h"
#include "Global.h"

SMatrix ZSpatialEntity::GetObjectToWorldMatrix() const
{
	return Function::CallRVOMethodAndReturn<SMatrix, const ZSpatialEntity*>(moduleBaseAddresses["runtime.entity.dll"] + 0x49710, this);
}
