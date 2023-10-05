#pragma once

#include <Common.h>

class IModule;
class ZString;
class IIniFile;
enum ERunMode;

class HitmanAbsolutionSDK_API IApplication
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
