#pragma once

#include "ZSimpleModuleBase.h"
#include "ZConfiguration.h"
#include "../Scene/ZEntitySceneContext.h"

#include <Common.h>

class ZHitman5Module : public ZSimpleModuleBase, public ZConfiguration
{
private:
	PAD(0x44);
	ZEntitySceneContext* m_pSceneContext;
};
