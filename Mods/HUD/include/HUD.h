#pragma once

#include <Glacier/UI/EHUDItem.h>
#include <Glacier/SGameUpdateEvent.h>
#include <Glacier/Input/ZInputAction.h>

#include <ModInterface.h>

class HUD : public ModInterface
{
public:
	HUD();

	void OnEngineInitialized() override;
	void OnDrawMenu() override;
	void OnDrawUI(const bool hasFocus) override;

	const bool IsHUDItemVisible(const char* name);
	void ToggleHUDItem(const char* name, const bool show);

private:
	void OnFrameUpdate(const SGameUpdateEvent& updateEvent);

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
	ZInputAction toggleHUDAction;
};

DECLARE_MOD(HUD)
