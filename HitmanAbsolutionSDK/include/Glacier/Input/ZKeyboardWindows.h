#pragma once

#include "ZKeyboardDevice.h"
#include "ZDirectInputInf.h"

class ZKeyboardWindows : public ZKeyboardDevice
{
public:
	virtual ~ZKeyboardWindows() = 0;
	void PrintInfo() = 0;
	bool Connected() = 0;
	int Initialize(bool bBackground) = 0;
	int Terminate() = 0;
	void Update(bool bIgnoreOldEvents) = 0;
	int Acquire() = 0;
	int Acquire(int retries) = 0;
	int Unacquire() = 0;
	char* lookupDeviceName(ZInputDevice::CtrlInfo* list, int nr) const = 0;

private:
	ZDirectInputInf dif;
	DIDEVICEOBJECTDATA m_pKeyboardData[256];
	short m_iLastPressed;
	short m_iLastMask;
	float m_fActivation;
};

static_assert(sizeof(ZKeyboardWindows) == 0x16B0);
static_assert(alignof(ZKeyboardWindows) == 0x8);
