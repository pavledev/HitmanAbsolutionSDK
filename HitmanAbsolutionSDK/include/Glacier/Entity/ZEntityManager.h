#pragma once

#include "../IComponentInterface.h"
#include "../Scene/IEntitySceneContext.h"
#include "../Entity/ZEntityType.h"

class ZEntityManager : public IComponentInterface
{
public:
	const IEntityBlueprintFactory* GetBlueprintFromCreatedEntity(ZEntityType** pEntity) const;

private:
	IEntitySceneContext* m_pSceneContext;
	unsigned int m_nTransformParentPropertyID;
	unsigned int m_nObjectToParentID;
};
