#pragma once

#include "../Templates/TArray.h"
#include "../Templates/TEntityRef.h"
#include "../Item/ZHM5Item.h"

class ZInventorySlot;

class HitmanAbsolutionSDK_API ZHM5BaseInventory
{
public:
	ZInventorySlot* AddItemToInventory(const TEntityRef<IHM5Item>& rItem, bool bAddInitialBullets, bool bDisableHUD);

private:
	TArray<TEntityRef<IHM5Item>> m_Items;
};
