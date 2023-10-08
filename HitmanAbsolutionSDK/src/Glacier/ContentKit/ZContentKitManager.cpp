#include <Glacier/ContentKit/ZContentKitManager.h>

const ZContentKitList<ZFireArmKitEntity, ZFireArmKitMenuSetup, SFireArmKitSaveData, SFireArmKitState>& ZContentKitManager::GetFireArms() const
{
	return m_FireArms;
}

const ZContentKitList<ZOutfitKitEntity, ZOutfitKitMenuSetup, SOutfitKitSaveData, SOutfitKitState>& ZContentKitManager::GetOutfits() const
{
	return m_Outfits;
}
