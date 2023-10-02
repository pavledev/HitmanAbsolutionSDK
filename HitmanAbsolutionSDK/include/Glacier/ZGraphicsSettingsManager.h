#pragma once

#include <Windows.h>

#include "IComponentInterface.h"

#include <Common.h>

class HitmanAbsolutionSDK_API ZGraphicsSettingsManager : public IComponentInterface
{
public:
	HWND GetHWND();

private:
	HWND m_hWnd; //0x4
	PAD(0x80);
};

static_assert(sizeof(ZGraphicsSettingsManager) == 0x88);
static_assert(alignof(ZGraphicsSettingsManager) == 0x4);
