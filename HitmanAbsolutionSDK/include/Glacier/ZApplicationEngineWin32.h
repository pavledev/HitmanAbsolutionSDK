#pragma once

#include <Windows.h>

#include "Utility/Alignment.h"

class IModule;
class ZString;
class IIniFile;
enum ERunMode;

class IApplication
{
public:
	virtual ~IApplication() = default;
	virtual void OnBeforeInitialize() = 0;
	virtual void OnAfterInitialize() = 0;
	virtual void OnBeforeShutdown() = 0;
	virtual void OnAfterShutdown() = 0;
	virtual void OnBeforeModuleInitialize(IModule* module) = 0;
	virtual void OnAfterModuleInitialize(IModule* module) = 0;
	virtual void OnBeforeModuleShutdown(IModule* module) = 0;
	virtual void OnAfterModuleShutdown(IModule* module) = 0;
	virtual void OnModuleNotFound(const ZString& string) = 0;
	virtual void SetOption(const ZString& string, const ZString& string2) = 0;
	virtual ZString GetOption(const ZString& string) = 0;
	virtual bool GetOptionBool(const ZString& string) = 0;
	virtual IIniFile* GetIniFile() = 0;
	virtual ERunMode GetRunMode() = 0;
	virtual void Exit() = 0;
	virtual void SetDebugContextInfo(const ZString& string, const ZString& string2) = 0;
	virtual ZString GetDebugContextInfo(const ZString& string) = 0;
	virtual bool IsNonInteractiveMode() = 0;
	virtual void SendErrorMail(const ZString& string, const ZString& string2) = 0;
	virtual void GetErrorMailSenderName(char* param1) = 0;
};

class ZApplicationBase : public IApplication
{
public:
	virtual ~ZApplicationBase() = 0;
	virtual ERunMode GetRunMode() = 0;
	virtual void SetDebugContextInfo(const ZString& sKey, const ZString& sValue) = 0;
	virtual ZString GetDebugContextInfo(const ZString& sKey) = 0;
	virtual bool IsNonInteractiveMode() = 0;
	virtual void SendErrorMail(const ZString& sChannel, const ZString& sMsg) = 0;

private:
	PAD(0x14);
};

class ZWin32ApplicationStub : public ZApplicationBase
{
public:
	virtual ~ZWin32ApplicationStub() = 0;
	virtual void OnBeforeInitialize() = 0;
	virtual void OnAfterInitialize() = 0;
	virtual void OnBeforeShutdown() = 0;
	virtual void OnAfterShutdown() = 0;
	virtual void OnBeforeModuleInitialize(IModule* pModule) = 0;
	virtual void OnAfterModuleInitialize(IModule* pModule) = 0;
	virtual void OnBeforeModuleShutdown(IModule* pModule) = 0;
	virtual void OnAfterModuleShutdown(IModule* pModule) = 0;
	virtual void OnModuleNotFound(const ZString& pszModuleName) = 0;
	virtual void SetOption(const ZString& sOption, const ZString& sValue) = 0;
	virtual ZString GetOption(const ZString& sOption) = 0;
	virtual bool GetOptionBool(const ZString& pszName) = 0;
	virtual IIniFile* GetIniFile() = 0;
	virtual void Exit() = 0;
	virtual void GetErrorMailSenderName(char* pBuffer) = 0;
	virtual void Initialize(const ZString& sDefaultSettings) = 0;
	virtual void AddApplicationSpecificOptions(IIniFile* pIniFile) = 0;
	virtual bool Startup() = 0;
	virtual void Shutdown() = 0;

private:
	PAD(0x2C);
};

class ZWin32ApplicationBase : public ZWin32ApplicationStub
{
public:
	virtual ~ZWin32ApplicationBase() = 0;
	virtual bool IsNonInteractiveMode() = 0;
	virtual void GetErrorMailSenderName(char* defFromBuf) = 0;
	virtual void AddApplicationSpecificOptions(IIniFile* pIniFile) = 0;
	virtual int Main(int param1, char** param2) = 0;

private:
	PAD(0x1C);
};

class alignas(8) ZApplicationEngineWin32 : public ZWin32ApplicationBase
{
public:
	HWND GetHWND();

private:
	PAD(0xF4);
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
