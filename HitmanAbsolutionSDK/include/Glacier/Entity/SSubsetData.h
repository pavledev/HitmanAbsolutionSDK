#pragma once

#include "../ZString.h"
#include "../TypeInfo/STypeID.h"
#include "../Templates/TArray.h"
#include "../Templates/TPair.h"

struct SSubsetData
{
	ZString m_sSubsetName;
	unsigned int m_nSubsetFlags;
	STypeID* m_subsetType;
	TArray<int> m_aEntityOffsets;
	TArray<TPair<int, SSubsetData*>> m_aEntitySubsets;
};
