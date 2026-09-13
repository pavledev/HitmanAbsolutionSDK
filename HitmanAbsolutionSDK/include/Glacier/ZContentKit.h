#pragma once

#include "ZEntity.h"
#include "ZResource.h"
#include "ZToken.h"
#include "TMap.h"
#include "TSet.h"
#include "ZString.h"
#include "ZActor.h"

class ZTextListData;
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
class ZHM5WeaponBasicConfigEntity;
class ZContentKitMenuSetup;

class ZContentKitEntity : public ZEntityImpl
{
  public:
    const STokenID& GetTokenID() const
    {
        ZTokenIDEntity* tokenIDEntity = m_Token.m_pInterfaceRef;

        if (tokenIDEntity)
        {
            return tokenIDEntity->GetTokenID();
        }

        return STokenID::InvalidToken;
    }

    enum EShowContent
    {
        YES = 0,
        NO = 1,
        ONLY_WHEN_UNLOCKED = 2
    };

    TEntityRef<ZTokenIDEntity> m_Token;
    ZString m_TitleLoganID;
    ZString m_DescriptionLoganID;
    TResourcePtr<ZTextListData> m_pTextListResource;
    ZString m_sCachedTitle;
    ZString m_sCachedDescription;
    EShowContent m_eShowInNotebook;
    EShowContent m_eShowOnCampaignEndScreen;
    EShowContent m_eShowOnContractsEndScreen;
    EShowContent m_eShowInSafeHouse;
    EShowContent m_eShowInGearSelection;
    bool m_bStartsLocked;
    int32_t m_iUnlockPrice;
};

class ZShadowContentKitList
{
  public:
    struct SEntry
    {
        const TEntityRef<ZContentKitEntity> GetFirstKit() const
        {
            return m_rKits.Begin().Node().m_data;
        }

        TSet<TEntityRef<ZContentKitEntity>> m_rKits;
        TEntityRef<ZContentKitMenuSetup> m_rMenu;
        STokenID m_CachedToken;
    };

    TArray<SEntry> m_List;
};

template<typename A, typename B, typename C, typename D> class ZContentKitList
{
  public:
    ZShadowContentKitList m_List;
    TMap<STokenID, D> m_PersistentState;
};

class ZFireArmKitEntity : public ZContentKitEntity
{
  public:
    ZRuntimeResourceID GetLargeIconRID() const
    {
        return m_sHiResNotebookImage;
    }

    TEntityRef<ZHM5WeaponBasicConfigEntity> m_BasicConfig;
    ZRuntimeResourceID m_sHiResNotebookImage;
    bool m_bIsNewPickup;
};

class ZGlobalOutfitKit : public ZEntityImpl
{
  public:
    ZRuntimeResourceID GetVariationResource(int32_t p_Variation)
    {
        ZRuntimeResourceID result;

        switch (p_Variation)
        {
        case 1:
            result = m_OutfitResource1;
            break;
        case 2:
            result = m_OutfitResource2;
            break;
        case 3:
            result = m_OutfitResource3;
            break;
        case 4:
            result = m_OutfitResource4;
            break;
        case 5:
            result = m_OutfitResource5;
            break;
        case 6:
            result = m_OutfitResource6;
            break;
        case 7:
            result = m_OutfitResource7;
            break;
        case 8:
            result = m_OutfitResource8;
            break;
        case 9:
            result = m_OutfitResource9;
            break;
        case 10:
            result = m_OutfitResource10;
            break;
        case 11:
            result = m_OutfitResource11;
            break;
        case 12:
            result = m_OutfitResource12;
            break;
        }

        return result;
    }

    STokenID m_TokenId;
    TResourcePtr<ZEntityRef> m_TokenResource;
    ZRuntimeResourceID m_OutfitResource1;
    ZRuntimeResourceID m_OutfitResource2;
    ZRuntimeResourceID m_OutfitResource3;
    ZRuntimeResourceID m_OutfitResource4;
    ZRuntimeResourceID m_OutfitResource5;
    ZRuntimeResourceID m_OutfitResource6;
    ZRuntimeResourceID m_OutfitResource7;
    ZRuntimeResourceID m_OutfitResource8;
    ZRuntimeResourceID m_OutfitResource9;
    ZRuntimeResourceID m_OutfitResource10;
    ZRuntimeResourceID m_OutfitResource11;
    ZRuntimeResourceID m_OutfitResource12;
};

class ZOutfitKitEntity : public ZContentKitEntity
{
  public:
    ZRuntimeResourceID GetLargeIconRID() const
    {
        return m_sHiResNotebookImage;
    }

    bool m_bIsNewPickup;
    SActorSoundDefs::EDefinition m_eSpeakSound;
    int m_nRatingAgainstSame;
    int m_nRating;
    int m_nArmorRating;
    bool m_bWeaponsAllowed;
    bool m_bAuthorityFigure;
    TArray<TEntityRef<ZTokenIDEntity>> m_aAllowedWeapons;
    ZRuntimeResourceID m_sHiResNotebookImage;
};

class ZPropKitEntity : public ZContentKitEntity
{
  public:
    ZRuntimeResourceID GetLargeIconRID() const
    {
        return m_sHiResNotebookImage;
    }

    ZRuntimeResourceID m_sHiResNotebookImage;
    bool m_bIsNewPickup;
};

class ZContentKitManager : public IComponentInterface
{
  public:
    struct SRuntimeItemResource
    {
        ZResourcePtr pResource;
        uint32_t nRefCount;
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
