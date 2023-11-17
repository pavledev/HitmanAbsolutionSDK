#pragma once

#include "../Templates/TArray.h"
#include "../Templates/TEntityRef.h"
#include "../Templates/TFixedArray.h"
#include "../Item/ZHM5Item.h"

class ZInventorySlot;

class HitmanAbsolutionSDK_API ZHM5BaseInventory
{
public:
	ZInventorySlot* AddItemToInventory(const TEntityRef<IHM5Item>& rItem, bool bAddInitialBullets, bool bDisableHUD);
	void SetAmmoInPocket(TFixedArray<unsigned int, 8>& nAmmoInPocket);

private:
	TArray<TEntityRef<IHM5Item>> m_Items;
	TArray<TEntityRef<IHM5Item>> m_RemoteControlledItems;
	unsigned int m_nAmmoInPocket[8];
};
