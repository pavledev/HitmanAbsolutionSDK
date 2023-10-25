#pragma once

#include "IEntityFactory.h"
#include "ZAspectEntityBlueprintFactory.h"

class HitmanAbsolutionSDK_API ZAspectEntityFactory : public IEntityFactory
{
public:
	virtual ~ZAspectEntityFactory() = default;

	ZRuntimeResourceID GetAspectEntityRuntimeResourceID() const;

private:
	TArray<TResourcePtr<IEntityFactory>> m_factoryResources;
	TResourcePtr<ZAspectEntityBlueprintFactory> m_blueprintResource;
	ZRuntimeResourceID m_ridResource;
};
