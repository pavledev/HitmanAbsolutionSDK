#pragma once

#include "ZInputDeviceManager.h"
#include "ZVirtualControlWindows.h"

class ZInputDeviceManagerWindows : public ZInputDeviceManager
{
public:
	virtual ~ZInputDeviceManagerWindows() = 0;
	virtual ZVariantRef GetVariantRef() const = 0;
	virtual int AddRef() = 0;
	virtual int Release() = 0;
	virtual void* QueryInterface(STypeID* iid) = 0;
	virtual void CreateDevices(int nWindowHandle, bool bBackground) = 0;
	virtual void Preupdate() = 0;
	virtual int Update(bool bIgnoreOldEvents) = 0;
	virtual void DetectDeviceInput() = 0;
	virtual IInputDevice* GetLastUsedDevice() = 0;
	virtual bool IsLastDeviceChanged() = 0;

	ZVirtualControlWindows* GetComboInputDevice() const;

private:
	ZVirtualControlWindows* m_pComboInputDevice;
	PAD(0x24);
};

static_assert(sizeof(ZInputDeviceManagerWindows) == 0x9C0);
static_assert(alignof(ZInputDeviceManagerWindows) == 0x8);
