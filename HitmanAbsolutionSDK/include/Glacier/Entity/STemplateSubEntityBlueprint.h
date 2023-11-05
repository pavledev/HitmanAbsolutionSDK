#pragma once

#include "../ZString.h"
#include "../Templates/TArray.h"
#include "../Templates/TPair.h"
#include "SEntityTemplatePropertyAlias.h"
#include "SEntityTemplateReference.h"
#include "SEntityTemplateEntitySubset.h"

struct STemplateSubEntityBlueprint
{
	int parentIndex;
	int entityTypeResourceIndex;
	ZString entityName;
	TArray<SEntityTemplatePropertyAlias> propertyAliases;
	TArray<TPair<ZString, SEntityTemplateReference>> exposedEntities;
	TArray<TPair<ZString, int>> exposedInterfaces;
	TArray<TPair<ZString, SEntityTemplateEntitySubset>> entitySubsets;
};
