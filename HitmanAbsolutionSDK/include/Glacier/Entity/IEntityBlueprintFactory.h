#pragma once

#include "../IComponentInterface.h"
#include "ZEntityType.h"

struct SEntityStats;

class IEntityBlueprintFactory : public IComponentInterface
{
public:
	virtual ~IEntityBlueprintFactory() = 0;
	virtual void GetMemoryRequirements(unsigned int* param1, unsigned int* param2, unsigned int* param3) const = 0;
	virtual ZEntityType* GetFactoryEntityType() const = 0;
	virtual ZEntityType** CreateEntity(unsigned char* param1, ZEntityType* entityType) = 0;
	virtual void DestroyEntity(ZEntityType** entityType) const = 0;
	virtual void DisableReloadOfConstAfterStartResources(ZEntityType** entityType) const = 0;
	virtual bool AreAllResourcesReady(ZEntityType** entityType) const = 0;
	virtual void Start(ZEntityType** entityType) const = 0;
	virtual void CollectInternalEntities(ZEntityType** entityType, TArray<ZEntityRef>& array) const = 0;
	virtual void CollectEntityStats(SEntityStats& entityStats) const = 0;
	virtual void ClearAllEntityReferences(ZEntityType** entityType) const = 0;
	virtual void ClearAllEntityReferencesOnChildren(ZEntityType** entityType) const = 0;
	virtual void OnOrphanedResource() = 0;
};
