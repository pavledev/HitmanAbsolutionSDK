#pragma once

#include "ZPhysicsBaseEntity.h"
#include "ICollisionShapeListener.h"
#include "IStaticPhysics.h"
#include "../Event/ZEvent.h"

class ZStaticPhysicsAspect : public ZPhysicsBaseEntity, public ICollisionShapeListener, public IStaticPhysics
{
private:
	bool m_bRemovePhysics;
	IPhysicsObject* m_pPhysicsObject;
	ZEvent<IPhysicsAccessor const*, IPhysicsObject const*, IPhysicsObject const*, ZEventNull, ZEventNull> m_physicsObjectListeners;
};
