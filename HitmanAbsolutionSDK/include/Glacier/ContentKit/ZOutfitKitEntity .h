#pragma once

#include "ZContentKitEntity.h"
#include "../Actor/SActorSoundDefs.h"
#include "TArray.h"
#include "ZRuntimeResourceID.h"

class ZTokenIDEntity;
template <typename T> class TEntityRef;

class ZOutfitKitEntity : public ZContentKitEntity
{
private:
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
