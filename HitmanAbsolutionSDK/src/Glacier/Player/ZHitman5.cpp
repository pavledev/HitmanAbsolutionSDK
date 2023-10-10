#include <Glacier/Player/ZHitman5.h>

ZHM5MainCamera* ZHitman5::GetMainCamera() const
{
	return m_rMainCamera.GetRawPointer();
}

ZHM5InputControl* ZHitman5::GetInputControl() const
{
	return m_pInputControl;
}

ZHM5FocusController* ZHitman5::GetFocusController() const
{
	return m_pFocusController;
}