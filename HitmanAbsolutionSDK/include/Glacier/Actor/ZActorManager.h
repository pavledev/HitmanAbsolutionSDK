#pragma once

#include "../IComponentInterface.h"
#include "../Templates/TArrayRef.h"
#include "../Templates/TMaxArray.h"
#include "../Templates/TEntityRef.h"
#include "ZActor.h"

class HitmanAbsolutionSDK_API ZActorManager : public IComponentInterface
{
public:
	TArrayRef<TEntityRef<ZActor>> GetAliveActors();

private:
	PAD(0x19668);
	TMaxArray<TEntityRef<ZActor>, 50> m_aliveActors; //0x1966C
	PAD(0x97D0);
};

static_assert(sizeof(ZActorManager) == 0x22FD0);
