#include <Glacier/ContentKit/ZShadowContentKitList.h>

const TEntityRef<ZContentKitEntity> ZShadowContentKitList::SEntry::GetFirstKit() const
{
	return m_rKits.Begin().Node().Data();
}

const TArray<ZShadowContentKitList::SEntry>& ZShadowContentKitList::GetEntries() const
{
	return m_List;
}
