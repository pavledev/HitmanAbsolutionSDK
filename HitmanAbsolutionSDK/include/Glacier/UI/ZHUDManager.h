#pragma once

#include "../IComponentInterface.h"
#include "IScaleformExternalInterfaceHandler.h"
#include "../Templates/TFixedArray.h"
#include "EHUDItem.h"
#include "ECrossHairType.h"
#include "EHUDFadeReason.h"
#include "IScaleformPlayer.h"

class HitmanAbsolutionSDK_API ZHUDManager : public IComponentInterface, public IScaleformExternalInterfaceHandler
{
public:
	void ShowHUD(bool bShow);
	void ShowTrespassingIcon(bool bShow);
	void ShowAIStateIcon(int nState);
	void ShowFocusBar(bool bShow);
	void ShowThreatRadar(bool bShow);
	bool IsPauseMenuActive();
	void ShowCrossHair();
	void HideCrossHair();
	void FadeHUDElements(unsigned int nElements, EHUDFadeReason eReason, bool bFadeIn, float fTime);
	void ScaleformShowWeaponDisplay(bool bShow);
	void HideRatingTracker();

	IScaleformPlayer* GetHUD() const;

private:
	PAD(0x4);
	IScaleformPlayer* m_mcHUD; //0xC
	PAD(0x30);
	TFixedArray<int, HUD_ITEM_NUM> m_aVisible; //0x40
	PAD(0x1);
	bool m_bDisplayHUD; //0x79
	PAD(0x2);
	bool m_bPauseMenuActive; //0x7C
	PAD(0x5A3);
	ECrossHairType m_eCrossHair; //0x620
};
