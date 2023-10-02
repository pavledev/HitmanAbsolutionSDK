#pragma once

#include <Windows.h>

#include "ZEngineAppCommon.h"

#include <Common.h>

class IModule;
class ZString;
class IIniFile;
enum ERunMode;

class HitmanAbsolutionSDK_API IApplication
{
public:
	virtual ~IApplication() = default;
};

class HitmanAbsolutionSDK_API ZApplicationBase : public IApplication
{
private:
	PAD(0x14);
};

class HitmanAbsolutionSDK_API ZWin32ApplicationStub : public ZApplicationBase
{
private:
	PAD(0x2C);
};

class HitmanAbsolutionSDK_API ZWin32ApplicationBase : public ZWin32ApplicationStub
{
private:
	PAD(0x1C);
};

class HitmanAbsolutionSDK_API alignas(8) ZApplicationEngineWin32 : public ZWin32ApplicationBase
{
public:
	static ZApplicationEngineWin32* GetInstance();
	static void SetInstance(ZApplicationEngineWin32* instance);
	ZEngineAppCommon& GetEngineAppCommon();
	HWND GetHWND();

private:
	PAD(0x10);
	ZEngineAppCommon m_common; //0x70
	PAD(0x64);
	HWND m_hWnd; //0x154
	PAD(0x20);

	inline static ZApplicationEngineWin32* m_pInstance = nullptr;
};

static_assert(sizeof(ZApplicationBase) == 0x18);
static_assert(alignof(ZApplicationBase) == 0x4);
static_assert(sizeof(ZWin32ApplicationStub) == 0x44);
static_assert(alignof(ZWin32ApplicationStub) == 0x4);
static_assert(sizeof(ZWin32ApplicationBase) == 0x60);
static_assert(alignof(ZWin32ApplicationBase) == 0x4);
static_assert(sizeof(ZApplicationEngineWin32) == 0x178);
static_assert(alignof(ZApplicationEngineWin32) == 0x8);
