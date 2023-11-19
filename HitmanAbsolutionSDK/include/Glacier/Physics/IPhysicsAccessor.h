#pragma once

#include "../IComponentInterface.h"
#include "../Templates/TEntityRef.h"
#include "../ZDelegate.h"
#include "IPhysicsObject.h"

class ZSpatialEntity;

class IPhysicsAccessor : public IComponentInterface
{
public:
	virtual ~IPhysicsAccessor() = 0;
	virtual TEntityRef<ZSpatialEntity> GetSpatialOwner() const = 0;
	virtual bool IsPhysicsObject() const = 0;
	virtual IPhysicsObject* GetPhysicsObject() const = 0;
	virtual bool IsRegisteredForPhysicsObjectListening(const ZDelegate<void __cdecl(IPhysicsAccessor const*, IPhysicsObject const*, IPhysicsObject const*)>& param1) const = 0;
	virtual void RegisterForPhysicsObjectListening(const ZDelegate<void __cdecl(IPhysicsAccessor const*, IPhysicsObject const*, IPhysicsObject const*)>& param1) = 0;
	virtual void UnregisterForPhysicsObjectListening(const ZDelegate<void __cdecl(IPhysicsAccessor const*, IPhysicsObject const*, IPhysicsObject const*)>& param1) = 0;
};
