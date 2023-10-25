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
