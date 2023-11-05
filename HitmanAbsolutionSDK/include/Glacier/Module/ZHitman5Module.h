#pragma once

#include "ZSimpleModuleBase.h"
#include "ZConfiguration.h"
#include "../Scene/ZEntitySceneContext.h"

#include <Common.h>

class HitmanAbsolutionSDK_API ZHitman5Module : public ZSimpleModuleBase, public ZConfiguration
{
public:
	ZEntitySceneContext* GetSceneContext();

private:
	PAD(0x44);
	ZEntitySceneContext* m_pSceneContext;
};
