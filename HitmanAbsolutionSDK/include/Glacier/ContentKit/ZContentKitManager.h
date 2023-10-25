#pragma once

#include "ZContentKitList.h"
#include "../IComponentInterface.h"
#include "../Resource/ZResourcePtr.h"
#include "../Templates/TEntityRef.h"
#include "ZFireArmKitEntity.h"
#include "ZOutfitKitEntity.h"
#include "ZPropKitEntity.h"
#include "ZGlobalOutfitKit.h"

class ZFireArmKitMenuSetup;
struct SFireArmKitSaveData;
struct SFireArmKitState;
class ZOutfitKitMenuSetup;
struct SOutfitKitSaveData;
struct SOutfitKitState;
class ZPropKitMenuSetup;
struct SPropKitSaveData;
struct SPropKitState;
class ZContentKitDLCInfo;
class ZGlobalItemKit;
class ZDLCDescriptor;

class HitmanAbsolutionSDK_API ZContentKitManager : public IComponentInterface
{
public:
	const ZContentKitList<ZFireArmKitEntity, ZFireArmKitMenuSetup, SFireArmKitSaveData, SFireArmKitState>& GetFireArms() const;
	const ZContentKitList<ZOutfitKitEntity, ZOutfitKitMenuSetup, SOutfitKitSaveData, SOutfitKitState>& GetOutfits() const;
	const ZContentKitList<ZPropKitEntity, ZPropKitMenuSetup, SPropKitSaveData, SPropKitState>& GetProps() const;
	const TMap<STokenID const, TEntityRef<ZGlobalOutfitKit>>& GetGlobalOutfitKits() const;

private:
	struct SRuntimeItemResource
	{
		ZResourcePtr pResource;
		unsigned int nRefCount;
	};

	ZContentKitList<ZFireArmKitEntity, ZFireArmKitMenuSetup, SFireArmKitSaveData, SFireArmKitState> m_FireArms;
	ZContentKitList<ZOutfitKitEntity, ZOutfitKitMenuSetup, SOutfitKitSaveData, SOutfitKitState> m_Outfits;
	ZContentKitList<ZPropKitEntity, ZPropKitMenuSetup, SPropKitSaveData, SPropKitState> m_Props;
	TMap<STokenID const, TEntityRef<ZContentKitDLCInfo>> m_KitDLCInfos;
	TMap<STokenID const, SRuntimeItemResource> m_RuntimeItemResources;
	TMap<STokenID const, TEntityRef<ZGlobalOutfitKit>> m_GlobalOutfitKits;
	TMap<STokenID const, TEntityRef<ZGlobalItemKit>> m_GlobalItemKits;
	TArray<TEntityRef<ZDLCDescriptor>> m_DLCDescriptors;
	TArray<STokenID> m_DLCUnlocksGranted;
};
