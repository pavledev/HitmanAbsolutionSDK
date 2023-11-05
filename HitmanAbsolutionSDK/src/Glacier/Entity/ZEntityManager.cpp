#include <Glacier/Entity/ZEntityManager.h>
#include <Glacier/Entity/ZEntityRef.h>

const IEntityBlueprintFactory* ZEntityManager::GetBlueprintFromCreatedEntity(ZEntityType** pEntity) const
{
	ZEntityRef entityRef = ZEntityRef(pEntity);
	void* interfacePtr = entityRef.QueryInterfacePtr<void>();

	if (!interfacePtr)
	{
		return nullptr;
	}

	void* vFTable = *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(interfacePtr) - sizeof(uintptr_t));

	if (vFTable == ZTemplateEntityBlueprintFactoryVFTbl)
	{
		return reinterpret_cast<IEntityBlueprintFactory*>(reinterpret_cast<uintptr_t>(interfacePtr) - sizeof(uintptr_t));
	}

	return nullptr;
}

IEntityBlueprintFactory* ZEntityManager::GetBlueprintFromCreatedEntity(ZEntityType** pEntity)
{
	ZEntityRef entityRef = ZEntityRef(pEntity);
	void* interfacePtr = entityRef.QueryInterfacePtr<void>();

	if (!interfacePtr)
	{
		return nullptr;
	}

	void* vFTable = *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(interfacePtr) - sizeof(uintptr_t));

	if (vFTable == ZTemplateEntityBlueprintFactoryVFTbl)
	{
		return reinterpret_cast<IEntityBlueprintFactory*>(reinterpret_cast<uintptr_t>(interfacePtr) - sizeof(uintptr_t));
	}

	return nullptr;
}
