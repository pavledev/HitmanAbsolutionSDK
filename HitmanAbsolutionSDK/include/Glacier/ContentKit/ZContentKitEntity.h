#pragma once

#include "../Entity/ZEntityImpl.h"
#include "../Templates/TEntityRef.h"
#include "../Templates/TResourcePtr.h"
#include "../ZString.h"
#include "../ZTokenIDEntity.h"

class ZTextListData;

class HitmanAbsolutionSDK_API ZContentKitEntity : public ZEntityImpl
{
public:
	const STokenID& GetTokenID() const;
	const ZString& GetLocalizedTitle() const;
	const ZString& GetLocalizedDescription() const;

private:
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
	int m_iUnlockPrice;
};
