#include <Glacier/Outfit/ZHM5Outfit.h>

TArrayRef<TEntityRef<ZTokenIDEntity>> ZHM5Outfit::GetOutfitTokens()
{
	return TArrayRef<TEntityRef<ZTokenIDEntity>>(m_aOutfitTokens.GetStart(), m_aOutfitTokens.Size());
}
