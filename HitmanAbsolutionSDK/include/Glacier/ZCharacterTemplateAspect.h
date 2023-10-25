#pragma once

#include "Entity/ZEntityImpl.h"
#include "Resource/ZResourcePtr.h"
#include "Resource/ZRuntimeResourceID.h"
#include "Templates/TEntityRef.h"
#include "Templates/TArray.h"
#include "Templates/TResourcePtr.h"
#include "SBodyPartDamageMultipliers.h"

class ZEntityRef;
class ZLinkedProxyEntity;
class ZString;
struct SSettingsParamMultiplier;
enum EActorCCPreset;

class ZCharacterTemplateAspect : public ZEntityImpl
{
public:
	ZRuntimeResourceID GetHMAsResourceID() const;

private:
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
