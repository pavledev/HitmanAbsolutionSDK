#include "Glacier/Camera/ZCameraEntity.h"

SMatrix ZCameraEntity::GetViewMatrix() const
{
	return GetObjectToWorldMatrix().Inverse();
}
