#pragma once

#include "IComponentInterface.h"
#include "ZEntity.h"
#include "ZResource.h"
#include "ZToken.h"

class IHM5Outfit : public IComponentInterface
{
  public:
    virtual bool IsAllowedIn(const ZEntityRef& entityRef) const = 0;
    virtual int32_t GetZoneMask() const = 0;
    virtual bool SupportsActor(const STokenID& tokenID) = 0;
};

class ZHM5Outfit : public ZEntityImpl, public IHM5Outfit
{
  public:
    int32_t m_iZoneMask;
    bool m_bZoneDefault;
    bool m_bZoneA;
    bool m_bZoneB;
    bool m_bZoneC;
    bool m_bZoneD;
    bool m_bZoneE;
    bool m_bZoneF;
    bool m_bZoneG;
    bool m_bZoneH;
    TArray<TEntityRef<ZTokenIDEntity>> m_aOutfitTokens;
    TResourcePtr<ZEntityRef> m_ContentKitEntity;
};

class ZOutfitManagerEntity : public ZEntityImpl
{
  public:
    TEntityRef<ZHM5Outfit> GetOutfit(STokenID p_OutfitToken)
    {
        for (size_t i = 0; i < m_Outfits.Size(); ++i)
        {
            for (size_t j = 0; j < m_Outfits[i].m_pInterfaceRef->m_aOutfitTokens.Size(); ++j)
            {
                if (m_Outfits[i].m_pInterfaceRef->m_aOutfitTokens[j].m_pInterfaceRef->GetTokenID() == p_OutfitToken)
                {
                    return m_Outfits[i];
                }
            }
        }

        return {};
    }

    TArray<TEntityRef<ZHM5Outfit>> m_Outfits;
};
