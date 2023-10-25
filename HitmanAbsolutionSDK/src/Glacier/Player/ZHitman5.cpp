#include <Glacier/Player/ZHitman5.h>

#include <Global.h>
#include <Function.h>

ZHM5MainCamera* ZHitman5::GetMainCamera() const
{
	return m_rMainCamera.GetRawPointer();
}

ZHM5InputControl* ZHitman5::GetInputControl() const
{
	return m_pInputControl;
}

ZHM5BaseInventory* ZHitman5::GetBaseInventory() const
{
	return m_pBaseInventory;
}

ZHM5FocusController* ZHitman5::GetFocusController() const
{
	return m_pFocusController;
}

void ZHitman5::EquipOutfitResource(ZRuntimeResourceID ridNewOutfitLibrary, const TEntityRef<IHM5Outfit> Outfit, bool bPlaceClothBundle, bool bForce)
{
	Function::CallMethod<ZHitman5*, ZRuntimeResourceID, const TEntityRef<IHM5Outfit>, bool, bool>(BaseAddress + 0x3B6390, this, ridNewOutfitLibrary, Outfit, bPlaceClothBundle, bForce);
}
