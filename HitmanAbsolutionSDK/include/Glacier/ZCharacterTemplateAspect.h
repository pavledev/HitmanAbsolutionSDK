#pragma once

#include "ZEntity.h"
#include "ZResource.h"

class ZLinkedProxyEntity;
struct SSettingsParamMultiplier;
enum EActorCCPreset;

struct SBodyPartDamageMultipliers
{
    float m_fHeadDamageMultiplier;
    float m_fFaceDamageMultiplier;
    float m_fArmDamageMultiplier;
    float m_fHandDamageMultiplier;
    float m_fLegDamageMultiplier;
    float m_fTorsoDamageMultiplier;
};

class ZCharacterTemplateAspect : public ZEntityImpl
{
  public:
    ZResourcePtr m_NudeResource;
    ZRuntimeResourceID m_HMAsResID;
    TArray<ZString> m_CharacterStatLabelList;
    EActorCCPreset m_eActorCCClass;
    bool m_bIsFemale;
    TEntityRef<ZLinkedProxyEntity> m_NudeEntity;
    TResourcePtr<ZEntityRef> m_sHUDPortraitShared;
    float m_fHitPoints;
    SBodyPartDamageMultipliers m_DamageMultipliers;
    bool m_bAllowRadioAct;
    bool m_bDisableRating;
    TArray<SSettingsParamMultiplier> m_SettingsParamMultipliers;
    bool m_bStreamedIn : 1;
    bool m_bIsNude : 1;
};
