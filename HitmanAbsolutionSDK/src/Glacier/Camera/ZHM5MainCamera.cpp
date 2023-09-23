#include "Glacier/Camera/ZHM5MainCamera.h"
#include "Function.h"
#include "Global.h"

void ZHM5MainCamera::SetCameraDirection(const float4& vLookTo)
{
	return Function::CallMethod<ZHM5MainCamera*, const float4&>(moduleBaseAddresses["hitman5Dll.dll"] + 0x3AF0A0, this, vLookTo);
}
