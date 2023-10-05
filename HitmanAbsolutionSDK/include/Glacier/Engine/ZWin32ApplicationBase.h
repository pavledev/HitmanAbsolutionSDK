#pragma once

#include "ZWin32ApplicationStub.h"

class HitmanAbsolutionSDK_API ZWin32ApplicationBase : public ZWin32ApplicationStub
{
public:
	virtual int Main(int param1, char** param2) = 0;

private:
	PAD(0x1C);
};
