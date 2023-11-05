#pragma once

#include "ZCompositeEntityBlueprintFactoryBase.h"
#include "../Resource/ZRuntimeResourceID.h"

struct SEntityProxyData;

class HitmanAbsolutionSDK_API ZAspectEntityBlueprintFactory : public ZCompositeEntityBlueprintFactoryBase
{
public:
	virtual ~ZAspectEntityBlueprintFactory() = default;

	IEntityBlueprintFactory* GetAspectFactory(const unsigned int index) const;

private:
	ZEntityType* m_pFactoryEntityType;
	TArray<IEntityBlueprintFactory*> m_aspectFactories;
	SEntityProxyData* m_pEntityProxyData;
	ZRuntimeResourceID m_ridResource;
};
