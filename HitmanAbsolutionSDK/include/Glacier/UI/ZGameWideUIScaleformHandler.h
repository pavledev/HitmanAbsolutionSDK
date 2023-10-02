#pragma once

#include "../Entity/ZEntityImpl.h"
#include "IScaleformExternalInterfaceHandler.h"

class HitmanAbsolutionSDK_API ZGameWideUIScaleformHandler : public ZEntityImpl, public IScaleformExternalInterfaceHandler
{
public:
	void ShowUICursor(bool bShow);
};
