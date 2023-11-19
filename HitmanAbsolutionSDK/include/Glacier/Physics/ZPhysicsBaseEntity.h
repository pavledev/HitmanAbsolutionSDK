#pragma once

#include "../Entity/ZEntityImpl.h"
#include "../Entity/ZPresenceState.h"

class ZPhysicsBaseEntity : public ZEntityImpl, public ZPresenceState
{
public:
	virtual ~ZPhysicsBaseEntity() = 0;
	virtual void OnPhysicsWorldStateChanged() = 0;
};
