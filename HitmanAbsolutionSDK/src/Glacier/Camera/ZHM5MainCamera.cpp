#include "Glacier/Camera/ZHM5MainCamera.h"

#include "Function.h"
#include "Global.h"

void ZHM5MainCamera::SetCameraDirection(const float4& vLookTo)
{
	return Function::CallMethod<ZHM5MainCamera*, const float4&>(BaseAddress + 0x5CACC0, this, vLookTo);
}
