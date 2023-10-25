#pragma once

#include "../Entity/ZEntityImpl.h"
#include "IHM5Outfit.h"
#include "../Templates/TResourcePtr.h"
#include "../Templates/TArray.h"
#include "../Templates/TArrayRef.h"
#include "../Entity/ZEntityRef.h"
#include "../Templates/TEntityRef.h"
#include "../ZTokenIDEntity.h"

class ZHM5Outfit : public ZEntityImpl, public IHM5Outfit
{
public:
	TArrayRef<TEntityRef<ZTokenIDEntity>> GetOutfitTokens();

private:
	int m_iZoneMask;
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
