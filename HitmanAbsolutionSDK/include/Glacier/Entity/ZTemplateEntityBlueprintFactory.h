#pragma once

#include "ZCompositeEntityBlueprintFactoryBase.h"
#include "../ZBitArray.h"
#include "../Resource/ZRuntimeResourceID.h"

struct SPinInfo;

class ZTemplateEntityBlueprintFactory : public ZCompositeEntityBlueprintFactoryBase
{
public:
	virtual ~ZTemplateEntityBlueprintFactory() = 0;

	int GetSubEntitiesCount() const;
	ZEntityType** GetSubEntity(ZEntityType** pEntity, int index) const;
	ZEntityType* GetSubEntityType(unsigned int nSubEntity) const;

private:
	TArray<IEntityBlueprintFactory*> m_blueprintResources;
	TArray<ZEntityType*> m_aEntityTypes;
	ZBitArray m_aModifiedEntityType;
	int m_rootEntityIndex;
	TArray<SPinInfo> m_aPinInfos;
	ZRuntimeResourceID m_ridResource;
};
