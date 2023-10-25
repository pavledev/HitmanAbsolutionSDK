#pragma once

#include "../STokenID.h"
#include "ZHM5Outfit.h"

class HitmanAbsolutionSDK_API ZOutfitManagerEntity : public ZEntityImpl
{
public:
	TEntityRef<ZHM5Outfit> GetOutfit(STokenID outfitToken);

private:
	TArray<TEntityRef<ZHM5Outfit>> m_Outfits;
};
