#pragma once

#include "../Templates/TEntityRef.h"

class ZHitman5;

class HitmanAbsolutionSDK_API ZHM5FocusController
{
public:
	void SetFocus(float fFocus);

private:
	bool m_bIsGaining;
	float m_fFocusGainTechniqueModifier;
	float m_fFocusMaxTechniqueModifier;
	float* m_pFocus;
	float m_fAdjustFocus;
	float m_fPendingMinFocus;
	float m_fPendingFocus;
	TEntityRef<ZHitman5> m_pHitman;
};
