#pragma once

#include "IPhysicsAccessor.h"

class IStaticPhysics : public IPhysicsAccessor
{
public:
	virtual ~IStaticPhysics() = 0;
};
