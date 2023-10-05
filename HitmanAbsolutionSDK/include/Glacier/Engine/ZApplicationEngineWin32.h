#pragma once

#include <Windows.h>

#include "ZEngineAppCommon.h"
#include "ZWin32ApplicationBase.h"

class HitmanAbsolutionSDK_API ZApplicationEngineWin32 : public ZWin32ApplicationBase
{
public:
	static ZApplicationEngineWin32* GetInstance();
	static void SetInstance(ZApplicationEngineWin32** instance);
	ZEngineAppCommon& GetEngineAppCommon();
	HWND GetHWND();

private:
	PAD(0x10);
	ZEngineAppCommon m_common; //0x70
	PAD(0x64);
	HWND m_hWnd; //0x154
	PAD(0x20);

	inline static ZApplicationEngineWin32** m_pInstance = nullptr;
};

static_assert(sizeof(ZApplicationBase) == 0x18);
static_assert(alignof(ZApplicationBase) == 0x4);
static_assert(sizeof(ZWin32ApplicationStub) == 0x44);
static_assert(alignof(ZWin32ApplicationStub) == 0x4);
static_assert(sizeof(ZWin32ApplicationBase) == 0x60);
static_assert(alignof(ZWin32ApplicationBase) == 0x4);
static_assert(sizeof(ZApplicationEngineWin32) == 0x178);
