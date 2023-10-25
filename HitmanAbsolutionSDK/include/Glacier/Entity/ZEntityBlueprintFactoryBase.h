#pragma once

#include "IEntityBlueprintFactory.h"

class ZEntityBlueprintFactoryBase : public IEntityBlueprintFactory
{
public:
	virtual ~ZEntityBlueprintFactoryBase() = 0;
	virtual void OnOrphanedResource() = 0;

protected:
	unsigned int m_nNeededMemory;
	unsigned int m_nNeededAlignment;
	unsigned int m_nOffsetToType;
};
