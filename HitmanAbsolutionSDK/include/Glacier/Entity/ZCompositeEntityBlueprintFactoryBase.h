#pragma once

#include "ZEntityBlueprintFactoryBase.h"

class HitmanAbsolutionSDK_API ZCompositeEntityBlueprintFactoryBase : public ZEntityBlueprintFactoryBase
{
public:
	virtual ~ZCompositeEntityBlueprintFactoryBase() = default;

	int GetSubEntitiesCount() const;
	ZEntityType** GetSubEntity(ZEntityType** pRootEntity, unsigned int index) const;

protected:
	TArray<unsigned int> m_aSubEntityOffsets;
};
