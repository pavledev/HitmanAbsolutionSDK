#pragma once

#include "IEntityBlueprintFactory.h"

class HitmanAbsolutionSDK_API ZEntityBlueprintFactoryBase : public IEntityBlueprintFactory
{
public:
	virtual ~ZEntityBlueprintFactoryBase() = default;
	virtual void OnOrphanedResource() = 0;

protected:
	unsigned int m_nNeededMemory;
	unsigned int m_nNeededAlignment;
	unsigned int m_nOffsetToType;
};
