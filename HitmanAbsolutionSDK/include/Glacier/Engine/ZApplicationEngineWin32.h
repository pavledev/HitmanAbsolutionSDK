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
	HICON GetDefaultCursor();
	void SetShowingCursor(const bool showingCursor);

	static ZCameraEntity* GetActiveCamera();

private:
	PAD(0x10);
	ZEngineAppCommon m_common; //0x70
	PAD(0x64);
	HWND m_hWnd; //0x154
	bool m_bTerminateApplication; //0x158
	bool m_bShowingCursor; //0x159
	bool m_bTempWindowed; //0x15A
	bool m_bPauseGameLoop; //0x15B
	bool m_bGameLoopPaused; //0x15C
	int m_PreviousMasterVolume; //0x160
	bool m_bChangeMasterVolume; //0x164
	int m_TargetMasterVolume; //0x168
	bool m_bActive; //0x16C
	HICON m_hDefaultCursor; //0x170
	PAD(0x4);

	inline static ZApplicationEngineWin32** m_pInstance = nullptr;
};

static_assert(sizeof(ZApplicationBase) == 0x18);
static_assert(alignof(ZApplicationBase) == 0x4);
static_assert(sizeof(ZWin32ApplicationStub) == 0x44);
static_assert(alignof(ZWin32ApplicationStub) == 0x4);
static_assert(sizeof(ZWin32ApplicationBase) == 0x60);
static_assert(alignof(ZWin32ApplicationBase) == 0x4);
static_assert(sizeof(ZApplicationEngineWin32) == 0x178);
