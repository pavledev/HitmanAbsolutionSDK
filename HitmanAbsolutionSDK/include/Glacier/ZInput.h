#pragma once

#define DIRECTINPUT_VERSION 0x0800

#include <dinput.h>
#include <cstdint>

#include "IComponentInterface.h"
#include "ZGameTime.h"

#include <Common.h>
#include <Globals.h>
#include <Functions.h>

class ZString;
class ZActionMapTree;

class IInputDevice
{
  public:
    enum EDeviceType
    {
        eMOUSE_TYPE = 0,
        eKEYBOARD_TYPE = 1,
        eGAMECONTROL_TYPE = 2,
        eCOMBOCONTROL_TYPE = 4
    };

    virtual ~IInputDevice() = default;
    virtual EDeviceType Type() const = 0;
    virtual int DigitalCount() const = 0;
    virtual int DigitalState(int32_t param1) const = 0;
    virtual char* DigitalName(int32_t param1) const = 0;
    virtual int DigitalId(const char* param1) const = 0;
    virtual ZGameTime DigitalHist(int32_t param1, int32_t param2) const = 0;
    virtual int AnalogCount() const = 0;
    virtual float AnalogState(int32_t param1) const = 0;
    virtual float AnalogStateRaw(int32_t param1) const = 0;
    virtual char* AnalogName(int32_t param1) const = 0;
    virtual int AnalogId(const char* param1) const = 0;
    virtual float AnalogMotion(int32_t param1) const = 0;
    virtual void ResetTables(bool param1) = 0;
    virtual void ClearKey(int32_t param1) = 0;
    virtual bool HapticEnabled() = 0;
    virtual void RumbleSetPitch(int32_t param1, float param2) = 0;
    virtual bool StartHaptic(uint32_t param1, float param2, float param3, bool param4) = 0;
    virtual void StopHaptics() = 0;
    virtual void SetRumbleBGPitch(float param1, float param2) = 0;
    virtual void SetRumblePitchDirect(float param1, float param2) = 0;
    virtual void Activate() = 0;
    virtual void Deactivate() = 0;
    virtual char* GetName() = 0;
    virtual void PrintInfo() = 0;
    virtual bool Connected() = 0;
};

class ZInputDevice : public IInputDevice
{
  public:
    struct CtrlInfo
    {
        char* name;
        int32_t nr;
    };

    virtual ~ZInputDevice() = default;
    virtual IInputDevice::EDeviceType Type() const = 0;
    virtual int DigitalCount() const = 0;
    virtual int DigitalState(int32_t nr) const = 0;
    virtual char* DigitalName(int32_t nr) const = 0;
    virtual int DigitalId(const char* name) const = 0;
    virtual ZGameTime DigitalHist(int32_t nr, int32_t nth) const = 0;
    virtual int AnalogCount() const = 0;
    virtual float AnalogState(int32_t nr) const = 0;
    virtual float AnalogStateRaw(int32_t nr) const = 0;
    virtual char* AnalogName(int32_t nr) const = 0;
    virtual int AnalogId(const char* name) const = 0;
    virtual float AnalogMotion(int32_t nr) const = 0;
    virtual void ResetTables(bool bClearKeys) = 0;
    virtual void ClearKey(int32_t id) = 0;
    virtual bool HapticEnabled() = 0;
    virtual void RumbleSetPitch(int32_t axis, float p) = 0;
    virtual bool StartHaptic(uint32_t iEffect, float fTimeOffset, float fTimeScale, bool bRunAlways) = 0;
    virtual void StopHaptics() = 0;
    virtual void SetRumbleBGPitch(float fLeft, float fRight) = 0;
    virtual void SetRumblePitchDirect(float fLeft, float fRight) = 0;
    virtual void Activate() = 0;
    virtual void Deactivate() = 0;
    virtual char* GetName() = 0;
    virtual void PrintInfo() = 0;
    virtual int Initialize(bool bBackground) = 0;
    virtual int Terminate() = 0;
    virtual void Update(bool bIgnoreOldEvents) = 0;
    virtual void Preupdate() = 0;
    virtual int Acquire() = 0;
    virtual int Acquire(int32_t retries) = 0;
    virtual int Unacquire() = 0;
    virtual char* lookupDeviceName(CtrlInfo* list, int32_t nr) const = 0;

    char m_name[32];
    ZGameTime (*m_dighist)[2];
    bool* m_pbDigital;
    bool* m_pbDigitalTrap;
    int32_t m_digcount;
    CtrlInfo* m_diginf;
    int32_t m_diginfcount;
    float* m_anamotion;
    float* m_analog;
    float* m_anaraw;
    int32_t m_anacount;
    CtrlInfo* m_anainf;
    int32_t m_anainfcount;
    ZGameTime m_updatetime;
    bool m_active;
    bool m_connected;
    uint8_t m_iDeviceID;
    int32_t (*m_pHandler[2])(int32_t, int32_t, void*);
    void* m_pHandlerData[2];
    int32_t m_nExclusiveIndex;
    IInputDevice::EDeviceType m_type;
};

static_assert(sizeof(ZInputDevice) == 0x80);
static_assert(alignof(ZInputDevice) == 0x8);

class IInputDeviceManager : public IComponentInterface
{
  public:
    virtual ~IInputDeviceManager() = 0;
    virtual void CreateDevices(int32_t param1, bool param2) = 0;
    virtual void Preupdate() = 0;
    virtual int Update(IInputDevice::EDeviceType deviceType, bool param2) = 0;
    virtual int Update(bool param1) = 0;
    virtual void DetectDeviceInput() = 0;
    virtual IInputDevice* GetLastUsedDevice() = 0;
    virtual bool IsLastDeviceChanged() = 0;
    virtual void ResetTables(bool param1) = 0;
    virtual IInputDevice* GetDevice(int32_t param1) = 0;
    virtual ZString GetDeviceName(int32_t param1) = 0;
    virtual int GetDeviceIdByName(const char* param1) = 0;
    virtual int GetNthDevice(IInputDevice::EDeviceType deviceType, int32_t param2) = 0;
    virtual void SetDefaultGameControllerIndex(int32_t param1) = 0;
    virtual int DeviceCount(IInputDevice::EDeviceType deviceType) = 0;
    virtual int DeviceType(int32_t param1) = 0;
    virtual int GetPrimaryDevice(IInputDevice::EDeviceType deviceType) = 0;
    virtual IInputDevice* GetPrimaryDevicePtr(IInputDevice::EDeviceType deviceType) = 0;
    virtual IInputDevice* GetNthDevicePtr(IInputDevice::EDeviceType deviceType, int32_t param2) = 0;
    virtual int MapDigital(const char* param1, const char* param2, int32_t& param3, int32_t& param4) = 0;
    virtual int MapAnalog(const char* param1, const char* param2, int32_t& param3, int32_t& param4) = 0;
    virtual float Analog(int32_t param1, int32_t param2) = 0;
    virtual float AnalogMotion(int32_t param1, int32_t param2) = 0;
    virtual int Digital(int32_t param1, int32_t param2) = 0;
    virtual void DeactivateDevices() = 0;
    virtual void ActivateDevices() = 0;
    virtual bool InstallHandler(int32_t param1, int32_t (*param2)(int32_t param1, int32_t param2, void* param3), void* param3, bool param4) = 0;
    virtual bool UninstallHandler(int32_t param1, int32_t (*param2)(int32_t param1, int32_t param2, void* param3)) = 0;
    virtual bool OpenControllerMissingWindow() = 0;
    virtual bool CheckForKeyPress(int32_t& param1, int32_t& param2) = 0;
    virtual bool HasGamepadConnected() = 0;
    virtual bool CheckKey(const char* param1, uint32_t param2) = 0;
    virtual bool DebugKeysEnabled() const = 0;
    virtual void EnableDebugKeys(bool param1) = 0;
    virtual void SetEnableHaptics(bool param1) = 0;
    virtual void TrapAllKeys() = 0;
};

class alignas(8) ZInputDeviceManager : public IInputDeviceManager
{
  public:
    virtual ~ZInputDeviceManager() = 0;
    virtual void Preupdate() = 0;
    virtual int32_t Update(IInputDevice::EDeviceType type, bool bIgnoreOldEvents) = 0;
    virtual int32_t Update(bool bIgnoreOldEvents) = 0;
    virtual void DetectDeviceInput() = 0;
    virtual IInputDevice* GetLastUsedDevice() = 0;
    virtual bool IsLastDeviceChanged() = 0;
    virtual void ResetTables(bool bClearKeys) = 0;
    virtual IInputDevice* GetDevice(int32_t id) = 0;
    virtual ZString GetDeviceName(int32_t id) = 0;
    virtual int32_t GetDeviceIdByName(const char* name) = 0;
    virtual int32_t GetNthDevice(IInputDevice::EDeviceType type, int32_t nth) = 0;
    virtual void SetDefaultGameControllerIndex(int32_t nIndex) = 0;
    virtual int32_t DeviceCount(IInputDevice::EDeviceType type) = 0;
    virtual int32_t DeviceType(int32_t id) = 0;
    virtual int32_t GetPrimaryDevice(IInputDevice::EDeviceType type) = 0;
    virtual IInputDevice* GetPrimaryDevicePtr(IInputDevice::EDeviceType type) = 0;
    virtual IInputDevice* GetNthDevicePtr(IInputDevice::EDeviceType type, int32_t nth) = 0;
    virtual int32_t MapDigital(const char* devname, const char* ctrlname, int32_t& devid, int32_t& ctrlid) = 0;
    virtual int32_t MapAnalog(const char* devname, const char* ctrlname, int32_t& devid, int32_t& ctrlid) = 0;
    virtual float Analog(int32_t devid, int32_t ctrlid) = 0;
    virtual float AnalogMotion(int32_t devid, int32_t ctrlid) = 0;
    virtual int32_t Digital(int32_t devid, int32_t ctrlid) = 0;
    virtual void DeactivateDevices() = 0;
    virtual void ActivateDevices() = 0;
    virtual bool
    InstallHandler(int32_t device, int32_t (*handler)(int32_t param1, int32_t param2, void* param3), void* userdata, bool bExclusive) = 0;
    virtual bool UninstallHandler(int32_t device, int32_t (*handler)(int32_t param1, int32_t param2, void* param3)) = 0;
    virtual bool OpenControllerMissingWindow() = 0;
    virtual bool CheckForKeyPress(int32_t& device, int32_t& controlid) = 0;
    virtual bool HasGamepadConnected() = 0;
    virtual bool CheckKey(const char* cKeyName, uint32_t eState) = 0;
    virtual bool DebugKeysEnabled() const = 0;
    virtual void EnableDebugKeys(bool bEnable) = 0;
    virtual void SetEnableHaptics(bool bEnableHaptics) = 0;
    virtual void TrapAllKeys() = 0;

  private:
    PAD(0x994);
};

static_assert(sizeof(ZInputDeviceManager) == 0x998);
static_assert(alignof(ZInputDeviceManager) == 0x8);

class ZVirtualControlWindows : public ZInputDevice
{
  public:
    PAD(0x1F8);
    ZInputDevice* m_Keyboard;
    ZInputDevice* m_Mouse;
};

static_assert(sizeof(ZVirtualControlWindows) == 0x280);
static_assert(alignof(ZVirtualControlWindows) == 0x8);

class ZInputDeviceManagerWindows : public ZInputDeviceManager
{
  public:
    virtual ~ZInputDeviceManagerWindows() = 0;
    virtual ZVariantRef GetVariantRef() const = 0;
    virtual int AddRef() = 0;
    virtual int Release() = 0;
    virtual void* QueryInterface(STypeID* iid) = 0;
    virtual void CreateDevices(int32_t nWindowHandle, bool bBackground) = 0;
    virtual void Preupdate() = 0;
    virtual int Update(bool bIgnoreOldEvents) = 0;
    virtual void DetectDeviceInput() = 0;
    virtual IInputDevice* GetLastUsedDevice() = 0;
    virtual bool IsLastDeviceChanged() = 0;

    ZVirtualControlWindows* m_pComboInputDevice;
    PAD(0x24);
};

static_assert(sizeof(ZInputDeviceManagerWindows) == 0x9C0);
static_assert(alignof(ZInputDeviceManagerWindows) == 0x8);

class ZDirectInputInf
{
  public:
    IDirectInput8A* m_pDI;
    IDirectInputDevice8A* m_pDev;
    GUID m_pGUIDInstance;
    HWND m_hWnd;
    bool m_bAcquired;
    char m_szDeviceName[260];
    char m_szProductName[260];
};

static_assert(sizeof(ZDirectInputInf) == 0x228);
static_assert(alignof(ZDirectInputInf) == 0x4);

class ZMouseDevice : public ZInputDevice
{
  public:
    bool m_bShowCursor;
};

static_assert(sizeof(ZMouseDevice) == 0x88);
static_assert(alignof(ZMouseDevice) == 0x8);

class ZMouseWindows : public ZMouseDevice
{
  public:
    virtual ~ZMouseWindows() = 0;
    virtual void PrintInfo() = 0;
    virtual bool Connected() = 0;
    virtual int Initialize(bool bBackground) = 0;
    virtual int Terminate() = 0;
    virtual void Update(bool bIgnoreOldEvents) = 0;
    virtual int Acquire() = 0;
    virtual int Acquire(int retries) = 0;
    virtual int Unacquire() = 0;
    virtual char* lookupDeviceName(ZInputDevice::CtrlInfo* list, int nr) const = 0;

    bool m_bSwapButtonsSchedule;
    bool m_bButtonSwapped;
    bool m_bWantedButtonSwap;
    bool m_bBackground;
    ZDirectInputInf dif;
    DIMOUSESTATE2 m_pMouseState;
    PAD(0x18);
};

static_assert(sizeof(ZMouseWindows) == 0x2E0);
static_assert(alignof(ZMouseWindows) == 0x8);

class ZKeyboardDevice : public ZInputDevice
{
  public:
    virtual ~ZKeyboardDevice() = 0;
};

static_assert(sizeof(ZKeyboardDevice) == 0x80);
static_assert(alignof(ZKeyboardDevice) == 0x8);

class ZKeyboardWindows : public ZKeyboardDevice
{
  public:
    virtual ~ZKeyboardWindows() = 0;
    virtual void PrintInfo() = 0;
    virtual bool Connected() = 0;
    virtual int Initialize(bool bBackground) = 0;
    virtual int Terminate() = 0;
    virtual void Update(bool bIgnoreOldEvents) = 0;
    virtual int Acquire() = 0;
    virtual int Acquire(int retries) = 0;
    virtual int Unacquire() = 0;
    virtual char* lookupDeviceName(ZInputDevice::CtrlInfo* list, int nr) const = 0;

    ZDirectInputInf dif;
    DIDEVICEOBJECTDATA m_pKeyboardData[256];
    short m_iLastPressed;
    short m_iLastMask;
    float m_fActivation;
};

static_assert(sizeof(ZKeyboardWindows) == 0x16B0);
static_assert(alignof(ZKeyboardWindows) == 0x8);

class ZInputAction
{
  public:
    ZInputAction() = default;

    ZInputAction(const char* p_Name) : m_szName(p_Name), m_pkMap(nullptr), m_iSeq(-1) {}

    float Analog()
    {
        return Functions::ZInputAction_Analog->Call(this);
    }

    bool Digital()
    {
        return Functions::ZInputAction_Digital->Call(this);
    }

    void SetEnabled(bool p_Enabled)
    {
        Functions::ZInputAction_SetEnabled->Call(this, p_Enabled);
    }

    const char* m_szName = nullptr;
    ZActionMapTree* m_pkMap = nullptr;
    int32_t m_iSeq = -1;
};

class ZHM5InputControl
{
  public:
    void EnableInputControl(bool p_Value)
    {
        m_bActive = p_Value;
    }

    void EnableBindings()
    {
        if (m_nBindingsDisabledCount-- == 1)
        {
            Globals::HM5InputControl->SetEnabled(true);
        }
    }

    void DisableBindings()
    {
        ++m_nBindingsDisabledCount;

        Globals::HM5InputControl->SetEnabled(false);
    }

    PAD(0xA8);
    bool m_bActive;                   // 0xA8
    int32_t m_nBindingsDisabledCount; // 0xAC
};

class ZInputActionManager : public IComponentInterface
{
  public:
    bool AddBindings(const char* p_Binds)
    {
        return Functions::ZInputActionManager_AddBindings->Call(this, p_Binds);
    }

    ZActionMapTree* m_pkRoot;
    ZActionMapTree* m_pkCurrentBlock;
    int32_t m_iBindMem;
    bool m_bDebugKeys;
    bool m_bEnabled;
    bool m_bSpeedRepeat;
    bool m_JoinControllers;
    ZGameTime m_EventHorizonDelay;
};
