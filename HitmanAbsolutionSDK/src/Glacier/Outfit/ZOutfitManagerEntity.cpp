#include <Glacier/Outfit/ZOutfitManagerEntity.h>

TEntityRef<ZHM5Outfit> ZOutfitManagerEntity::GetOutfit(STokenID outfitToken)
{
	for (size_t i = 0; i < m_Outfits.Size(); ++i)
	{
		TArrayRef<TEntityRef<ZTokenIDEntity>> outfitTokens = m_Outfits[i].GetRawPointer()->GetOutfitTokens();

		for (size_t j = 0; j < outfitTokens.Size(); ++j)
		{
			if (outfitTokens[j].GetRawPointer()->GetTokenID() == outfitToken)
			{
				return m_Outfits[i];
			}
		}
	}

	return TEntityRef<ZHM5Outfit>();
}
