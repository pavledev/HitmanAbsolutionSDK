#pragma once

#include "../IComponentInterface.h"
#include "IInputDevice.h"

class ZString;

class IInputDeviceManager : public IComponentInterface
{
public:
	virtual ~IInputDeviceManager() = 0;
	virtual void CreateDevices(int param1, bool param2) = 0;
	virtual void Preupdate() = 0;
	virtual int Update(IInputDevice::EDeviceType deviceType, bool param2) = 0;
	virtual int Update(bool param1) = 0;
	virtual void DetectDeviceInput() = 0;
	virtual IInputDevice* GetLastUsedDevice() = 0;
	virtual bool IsLastDeviceChanged() = 0;
	virtual void ResetTables(bool param1) = 0;
	virtual IInputDevice* GetDevice(int param1) = 0;
	virtual ZString GetDeviceName(int param1) = 0;
	virtual int GetDeviceIdByName(const char* param1) = 0;
	virtual int GetNthDevice(IInputDevice::EDeviceType deviceType, int param2) = 0;
	virtual void SetDefaultGameControllerIndex(int param1) = 0;
	virtual int DeviceCount(IInputDevice::EDeviceType deviceType) = 0;
	virtual int DeviceType(int param1) = 0;
	virtual int GetPrimaryDevice(IInputDevice::EDeviceType deviceType) = 0;
	virtual IInputDevice* GetPrimaryDevicePtr(IInputDevice::EDeviceType deviceType) = 0;
	virtual IInputDevice* GetNthDevicePtr(IInputDevice::EDeviceType deviceType, int param2) = 0;
	virtual int MapDigital(const char* param1, const char* param2, int& param3, int& param4) = 0;
	virtual int MapAnalog(const char* param1, const char* param2, int& param3, int& param4) = 0;
	virtual float Analog(int param1, int param2) = 0;
	virtual float AnalogMotion(int param1, int param2) = 0;
	virtual int Digital(int param1, int param2) = 0;
	virtual void DeactivateDevices() = 0;
	virtual void ActivateDevices() = 0;
	virtual bool InstallHandler(int param1, int (*param2)(int param1, int param2, void* param3), void* param3, bool param4) = 0;
	virtual bool UninstallHandler(int param1, int (*param2)(int param1, int param2, void* param3)) = 0;
	virtual bool OpenControllerMissingWindow() = 0;
	virtual bool CheckForKeyPress(int& param1, int& param2) = 0;
	virtual bool HasGamepadConnected() = 0;
	virtual bool CheckKey(const char* param1, unsigned int param2) = 0;
	virtual bool DebugKeysEnabled() const = 0;
	virtual void EnableDebugKeys(bool param1) = 0;
	virtual void SetEnableHaptics(bool param1) = 0;
	virtual void TrapAllKeys() = 0;
};
