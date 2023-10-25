#pragma once

#include "../Entity/ZEntityImpl.h"
#include "EActorCCPreset.h"

class HitmanAbsolutionSDK_API ZHM5CCProfile : public ZEntityImpl
{
public:
	static ZHM5CCProfile* GetDefaultCCProfile(EActorCCPreset eActorCCPreset);

private:
	int m_nAttacksForTakeDown;
	float m_fTakeDownWindow;
	float m_fTakeDownWindowIncFraction;
	float m_fHitmanDamage;
	bool m_bAllowAttackingHitman;
	float m_fDecayTime;
	int m_nKillMashNum;
	int m_nNumChainsToKill;
	EActorCCPreset m_eActorCCPreset;

	inline static ZHM5CCProfile* s_apPresetCCProfile[9];
};
