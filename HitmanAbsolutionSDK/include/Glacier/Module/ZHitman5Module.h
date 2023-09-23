#pragma once

#include "ZSimpleModuleBase.h"
#include "ZConfiguration.h"

#include <Utility/Alignment.h>

class ZEntitySceneContext;

class ZHitman5Module : public ZSimpleModuleBase, public ZConfiguration
{
public:
	const bool IsEngineInitialized() const;

private:
	PAD(0x44);
	ZEntitySceneContext* m_pSceneContext;
};
