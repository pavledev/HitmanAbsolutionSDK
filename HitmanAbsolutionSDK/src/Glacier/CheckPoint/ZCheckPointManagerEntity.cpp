#include <Glacier/CheckPoint/ZCheckPointManagerEntity.h>

#include <Function.h>
#include <Global.h>

void ZCheckPointManagerEntity::ActivateJumpPoint(int iIndex, bool bResetHitman)
{
	Function::CallMethod<ZCheckPointManagerEntity*, int, bool>(BaseAddress + 0x388C50, this, iIndex, bResetHitman);
}

const int ZCheckPointManagerEntity::GetCurrentJumpPoint() const
{
	return m_iCurrentJumpPoint;
}
