#include <Glacier/Entity/ZCompositeEntityBlueprintFactoryBase.h>

int ZCompositeEntityBlueprintFactoryBase::GetSubEntitiesCount() const
{
	return m_aSubEntityOffsets.Size();
}

ZEntityType** ZCompositeEntityBlueprintFactoryBase::GetSubEntity(ZEntityType** pRootEntity, unsigned int index) const
{
	return reinterpret_cast<ZEntityType**>(reinterpret_cast<uintptr_t>(pRootEntity) + m_aSubEntityOffsets[index]);
}
