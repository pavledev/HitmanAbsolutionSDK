#pragma once

#include "../Templates/TArray.h"

struct STypeID;

struct SEntityTemplateEntitySubset
{
	STypeID* subsetType;
	TArray<int> entities;
};
