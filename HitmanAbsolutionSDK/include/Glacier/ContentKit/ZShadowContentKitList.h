#pragma once

#include "../Templates/TArray.h"
#include "../Templates/TEntityRef.h"
#include "../STokenID.h"
#include "../Templates/TSet.h"
#include "ZContentKitEntity.h"

class ZContentKitMenuSetup;

class HitmanAbsolutionSDK_API ZShadowContentKitList
{
public:
	struct HitmanAbsolutionSDK_API SEntry
	{
		const TEntityRef<ZContentKitEntity> GetFirstKit() const;

		TSet<TEntityRef<ZContentKitEntity>> m_rKits;
		TEntityRef<ZContentKitMenuSetup> m_rMenu;
		STokenID m_CachedToken;
	};

	const TArray<SEntry>& GetEntries() const;

private:
	TArray<SEntry> m_List;
};
