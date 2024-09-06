#include <Glacier/UI/ZHUDManager.h>

#include <Function.h>
#include "Global.h"

void ZHUDManager::ShowHUD(bool bShow)
{
	Function::CallMethod<ZHUDManager*, bool>(BaseAddress + 0x1C5690, this, bShow);
}

void ZHUDManager::ShowTrespassingIcon(bool bShow)
{
	Function::CallMethod<ZHUDManager*, bool>(BaseAddress + 0x1F9670, this, bShow);
}

void ZHUDManager::ShowAIStateIcon(int nState)
{
	Function::CallMethod<ZHUDManager*, int>(BaseAddress + 0x4CE8B0, this, nState);
}

void ZHUDManager::ShowFocusBar(bool bShow)
{
	if (m_bDisplayHUD && bShow != (m_aVisible[9] != 0))
	{
		FadeHUDElements(8, HUD_FADE_REASON_CODE_GENERAL, bShow, 0.f);

		m_aVisible[9] = bShow;
	}
}

void ZHUDManager::ShowThreatRadar(bool bShow)
{
	Function::CallMethod<ZHUDManager*, bool>(BaseAddress + 0x4BB670, this, bShow);
}

bool ZHUDManager::IsPauseMenuActive()
{
	return m_bPauseMenuActive;
}

void ZHUDManager::ShowCrossHair()
{
	m_eCrossHair = CROSSHAIR_PISTOL;
}

void ZHUDManager::HideCrossHair()
{
	m_eCrossHair = CROSSHAIR_NONE;
}

void ZHUDManager::FadeHUDElements(unsigned int nElements, EHUDFadeReason eReason, bool bFadeIn, float fTime)
{
	Function::CallMethod<ZHUDManager*, unsigned int, EHUDFadeReason, bool, float>(BaseAddress + 0xED060, this, nElements, eReason, bFadeIn, fTime);
}

void ZHUDManager::ScaleformShowWeaponDisplay(bool bShow)
{
	Function::CallMethod<ZHUDManager*, bool>(BaseAddress + 0x490A00, this, bShow);
}

void ZHUDManager::HideRatingTracker()
{
	Function::CallMethod<ZHUDManager*>(BaseAddress + 0x490A00, this);
}

IScaleformPlayer* ZHUDManager::GetHUD() const
{
	return m_mcHUD;
}
