#pragma once

#include "IInputDeviceManager.h"
#include "../ZGameTime.h"
#include "Utility/Alignment.h"

class ZInputDevice;

class alignas(8) ZInputDeviceManager : public IInputDeviceManager
{
public:
	virtual ~ZInputDeviceManager() = 0;
	virtual void Preupdate() = 0;
	virtual int Update(IInputDevice::EDeviceType type, bool bIgnoreOldEvents) = 0;
	virtual int Update(bool bIgnoreOldEvents) = 0;
	virtual void DetectDeviceInput() = 0;
	virtual IInputDevice* GetLastUsedDevice() = 0;
	virtual bool IsLastDeviceChanged() = 0;
	virtual void ResetTables(bool bClearKeys) = 0;
	virtual IInputDevice* GetDevice(int id) = 0;
	virtual ZString GetDeviceName(int id) = 0;
	virtual int GetDeviceIdByName(const char* name) = 0;
	virtual int GetNthDevice(IInputDevice::EDeviceType type, int nth) = 0;
	virtual void SetDefaultGameControllerIndex(int nIndex) = 0;
	virtual int DeviceCount(IInputDevice::EDeviceType type) = 0;
	virtual int DeviceType(int id) = 0;
	virtual int GetPrimaryDevice(IInputDevice::EDeviceType type) = 0;
	virtual IInputDevice* GetPrimaryDevicePtr(IInputDevice::EDeviceType type) = 0;
	virtual IInputDevice* GetNthDevicePtr(IInputDevice::EDeviceType type, int nth) = 0;
	virtual int MapDigital(const char* devname, const char* ctrlname, int& devid, int& ctrlid) = 0;
	virtual int MapAnalog(const char* devname, const char* ctrlname, int& devid, int& ctrlid) = 0;
	virtual float Analog(int devid, int ctrlid) = 0;
	virtual float AnalogMotion(int devid, int ctrlid) = 0;
	virtual int Digital(int devid, int ctrlid) = 0;
	virtual void DeactivateDevices() = 0;
	virtual void ActivateDevices() = 0;
	virtual bool InstallHandler(int device, int (*handler)(int param1, int param2, void* param3), void* userdata, bool bExclusive) = 0;
	virtual bool UninstallHandler(int device, int (*handler)(int param1, int param2, void* param3)) = 0;
	virtual bool OpenControllerMissingWindow() = 0;
	virtual bool CheckForKeyPress(int& device, int& controlid) = 0;
	virtual bool HasGamepadConnected() = 0;
	virtual bool CheckKey(const char* cKeyName, unsigned int eState) = 0;
	virtual bool DebugKeysEnabled() const = 0;
	virtual void EnableDebugKeys(bool bEnable) = 0;
	virtual void SetEnableHaptics(bool bEnableHaptics) = 0;
	virtual void TrapAllKeys() = 0;

private:
	PAD(0x994);
};

static_assert(sizeof(ZInputDeviceManager) == 0x998);
static_assert(alignof(ZInputDeviceManager) == 0x8);
