#include <Glacier/Entity/ZEntityManager.h>
#include <Glacier/Entity/ZEntityRef.h>

const IEntityBlueprintFactory* ZEntityManager::GetBlueprintFromCreatedEntity(ZEntityType** pEntity) const
{
	ZEntityRef entityRef = ZEntityRef(pEntity);

	return reinterpret_cast<IEntityBlueprintFactory*>(reinterpret_cast<uintptr_t>(entityRef.QueryInterfacePtr<void>()) - sizeof(uintptr_t));
}
