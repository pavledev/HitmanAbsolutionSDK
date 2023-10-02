#pragma once

#include "../IComponentInterface.h"
#include "IScaleformExternalInterfaceHandler.h"

class HitmanAbsolutionSDK_API ZHUDManager : public IComponentInterface, public IScaleformExternalInterfaceHandler
{
public:
	bool IsPauseMenuActive();

private:
	PAD(0x74);
	bool m_bPauseMenuActive; //0x7C
};
