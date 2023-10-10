#include <Glacier/Player/ZHitman5.h>

ZHM5MainCamera* ZHitman5::GetMainCamera() const
{
	return m_rMainCamera.GetRawPointer();
}

ZHM5InputControl* ZHitman5::GetInputControl() const
{
	return m_pInputControl;
}
