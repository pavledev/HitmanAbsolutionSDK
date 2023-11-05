#include <Glacier/Entity/ZTemplateEntityBlueprintFactory.h>

int ZTemplateEntityBlueprintFactory::GetSubEntitiesCount() const
{
	return m_blueprintResources.Size();
}

ZEntityType** ZTemplateEntityBlueprintFactory::GetSubEntity(ZEntityType** pEntity, int index) const
{
	return reinterpret_cast<ZEntityType**>(reinterpret_cast<uintptr_t>(pEntity) + m_aSubEntityOffsets[index]);
}

ZEntityType* ZTemplateEntityBlueprintFactory::GetSubEntityType(unsigned int nSubEntity) const
{
	return m_blueprintResources[nSubEntity]->GetFactoryEntityType();
}

IEntityBlueprintFactory* ZTemplateEntityBlueprintFactory::GetBlueprintResource(const unsigned int entityIndex) const
{
	return m_blueprintResources[entityIndex];
}

const int ZTemplateEntityBlueprintFactory::GetRootEntityIndex() const
{
	return m_rootEntityIndex;
}

const ZRuntimeResourceID& ZTemplateEntityBlueprintFactory::GetRuntimeResourceID() const
{
	return m_ridResource;
}
