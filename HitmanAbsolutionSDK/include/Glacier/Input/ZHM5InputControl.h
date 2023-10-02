#pragma once

#include <Common.h>

class HitmanAbsolutionSDK_API ZHM5InputControl
{
public:
	void EnableInputControl(bool bValue);
	void EnableBindings();
	void DisableBindings();

private:
	bool m_bActive; //0x0
	PAD(0xAB);
	int m_nBindingsDisabledCount; //0xAC
};
