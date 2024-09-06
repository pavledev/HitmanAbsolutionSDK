#pragma once

#include <array>

#include <Glacier/UI/EHUDItem.h>

#include <ModInterface.h>

class HUD : public ModInterface
{
public:
	void OnDrawMenu() override;
	void OnDrawUI(const bool hasFocus) override;

	const bool IsHUDItemVisible(const char* name);
	void ToggleHUDItem(const char* name, const bool show);

private:
	bool isOpen;

	inline static int* uiDisableHUD;
	bool isHUDVisible;
	bool isHealthBarVisible;
	bool isMinimapVisible;
	bool isTrespassingIconVisible;
	bool isWeaponDisplayVisible;
	bool isWeaponSelectorVisible;
	bool isFocusBarVisible;
	bool isRatingTrackerVisible;
	bool isTargetTrackerVisible;
	bool isAttentionVisible;
	bool isCrosshairVisible;
};

DECLARE_MOD(HUD)
