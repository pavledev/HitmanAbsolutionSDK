#include "Glacier/ZHitman5.h"

ZHM5MainCamera* ZHitman5::GetMainCamera() const
{
	return m_rMainCamera.GetRawPointer();
}
