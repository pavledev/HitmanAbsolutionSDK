#pragma once

#include "../IComponentInterface.h"
#include "../Templates/TEntityRef.h"
#include "ZGameWideUIScaleformHandler.h"

class HitmanAbsolutionSDK_API ZGameWideUI : public IComponentInterface
{
public:
	ZGameWideUIScaleformHandler* GetGameWideUIScaleformHandler();

private:
	TEntityRef<ZGameWideUIScaleformHandler> m_rGameWideUI; //0x4
};
