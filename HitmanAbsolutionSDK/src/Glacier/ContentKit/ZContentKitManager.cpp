#include <Glacier/ContentKit/ZContentKitManager.h>

const ZContentKitList<ZFireArmKitEntity, ZFireArmKitMenuSetup, SFireArmKitSaveData, SFireArmKitState>& ZContentKitManager::GetFireArms() const
{
	return m_FireArms;
}

const ZContentKitList<ZOutfitKitEntity, ZOutfitKitMenuSetup, SOutfitKitSaveData, SOutfitKitState>& ZContentKitManager::GetOutfits() const
{
	return m_Outfits;
}

const ZContentKitList<ZPropKitEntity, ZPropKitMenuSetup, SPropKitSaveData, SPropKitState>& ZContentKitManager::GetProps() const
{
	return m_Props;
}

const TMap<STokenID const, TEntityRef<ZGlobalOutfitKit>>& ZContentKitManager::GetGlobalOutfitKits() const
{
	return m_GlobalOutfitKits;
}
