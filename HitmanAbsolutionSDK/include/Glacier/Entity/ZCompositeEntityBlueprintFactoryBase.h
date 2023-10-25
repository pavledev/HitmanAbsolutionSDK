#pragma once

#include "ZEntityBlueprintFactoryBase.h"

class ZCompositeEntityBlueprintFactoryBase : public ZEntityBlueprintFactoryBase
{
public:
	virtual ~ZCompositeEntityBlueprintFactoryBase() = 0;

	int GetSubEntitiesCount() const;
	ZEntityType** GetSubEntity(ZEntityType** pRootEntity, unsigned int index) const;

protected:
	TArray<unsigned int> m_aSubEntityOffsets;
};
