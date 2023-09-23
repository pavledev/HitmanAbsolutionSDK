#pragma once

#include "ZMouseDevice.h"
#include "ZDirectInputInf.h"
#include "Utility/Alignment.h"

class ZMouseWindows : public ZMouseDevice
{
public:
	~ZMouseWindows() = 0;
	void PrintInfo() = 0;
	bool Connected() = 0;
	int Initialize(bool bBackground) = 0;
	int Terminate() = 0;
	void Update(bool bIgnoreOldEvents) = 0;
	int Acquire() = 0;
	int Acquire(int retries) = 0;
	int Unacquire() = 0;
	char* lookupDeviceName(ZInputDevice::CtrlInfo* list, int nr) const = 0;

	void Update();

private:
	bool m_bSwapButtonsSchedule;
	bool m_bButtonSwapped;
	bool m_bWantedButtonSwap;
	bool m_bBackground;
	ZDirectInputInf dif;
	DIMOUSESTATE2 m_pMouseState;
	PAD(0x18);
};

static_assert(sizeof(ZMouseWindows) == 0x2E0);
static_assert(alignof(ZMouseWindows) == 0x8);
