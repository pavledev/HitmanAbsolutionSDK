#pragma once

#include "ZCompositeEntityBlueprintFactoryBase.h"
#include "../Resource/ZRuntimeResourceID.h"

struct SEntityProxyData;

class ZAspectEntityBlueprintFactory : public ZCompositeEntityBlueprintFactoryBase
{
public:
	virtual ~ZAspectEntityBlueprintFactory() = default;

private:
	ZEntityType* m_pFactoryEntityType;
	TArray<IEntityBlueprintFactory*> m_aspectFactories;
	SEntityProxyData* m_pEntityProxyData;
	ZRuntimeResourceID m_ridResource;
};
