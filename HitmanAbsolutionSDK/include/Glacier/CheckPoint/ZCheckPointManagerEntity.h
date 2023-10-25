#pragma once

#include "../Entity/ZEntityImpl.h"

class HitmanAbsolutionSDK_API ZCheckPointManagerEntity : public ZEntityImpl
{
public:
	void ActivateJumpPoint(int iIndex, bool bResetHitman);
	const int GetCurrentJumpPoint() const;

private:
	PAD(0x84);
	int m_iCurrentJumpPoint;
};
