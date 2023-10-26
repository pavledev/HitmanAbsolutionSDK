#pragma once

#include <ModInterface.h>

class Items : public ModInterface
{
public:
	Items();
	void OnDrawMenu() override;
	void OnDrawUI(const bool hasFocus) override;

private:
	bool isOpen;
	int selectedItemIndex;
};

DECLARE_MOD(Items)
