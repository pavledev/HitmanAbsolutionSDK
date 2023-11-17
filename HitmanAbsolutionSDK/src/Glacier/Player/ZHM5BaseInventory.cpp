#include <Glacier/Player/ZHM5BaseInventory.h>

#include <Global.h>
#include <Function.h>

ZInventorySlot* ZHM5BaseInventory::AddItemToInventory(const TEntityRef<IHM5Item>& rItem, bool bAddInitialBullets, bool bDisableHUD)
{
	return Function::CallMethodAndReturn<ZInventorySlot*, ZHM5BaseInventory*, const TEntityRef<IHM5Item>&, bool, bool>(BaseAddress + 0x5476A0, this, rItem, bAddInitialBullets, bDisableHUD);
}

void ZHM5BaseInventory::SetAmmoInPocket(TFixedArray<unsigned int, 8>& nAmmoInPocket)
{
	*reinterpret_cast<TFixedArray<unsigned int, 8>*>(m_nAmmoInPocket) = nAmmoInPocket;
}
