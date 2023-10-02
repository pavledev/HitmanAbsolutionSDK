#pragma once

#include "IInputDevice.h"
#include "../ZGameTime.h"

class HitmanAbsolutionSDK_API ZInputDevice : public IInputDevice
{
public:
    struct CtrlInfo
    {
        char* name;
        int nr;
    };

    virtual ~ZInputDevice() = default;
    virtual IInputDevice::EDeviceType Type() const = 0;
    virtual int DigitalCount() const = 0;
    virtual int DigitalState(int nr) const = 0;
    virtual char* DigitalName(int nr) const = 0;
    virtual int DigitalId(const char* name) const = 0;
    virtual ZGameTime DigitalHist(int nr, int nth) const = 0;
    virtual int AnalogCount() const = 0;
    virtual float AnalogState(int nr) const = 0;
    virtual float AnalogStateRaw(int nr) const = 0;
    virtual char* AnalogName(int nr) const = 0;
    virtual int AnalogId(const char* name) const = 0;
    virtual float AnalogMotion(int nr) const = 0;
    virtual void ResetTables(bool bClearKeys) = 0;
    virtual void ClearKey(int id) = 0;
    virtual bool HapticEnabled() = 0;
    virtual void RumbleSetPitch(int axis, float p) = 0;
    virtual bool StartHaptic(unsigned int iEffect, float fTimeOffset, float fTimeScale, bool bRunAlways) = 0;
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
    virtual int Acquire(int retries) = 0;
    virtual int Unacquire() = 0;
    virtual char* lookupDeviceName(CtrlInfo* list, int nr) const = 0;

    bool HandlerInstalled();
    void SendToHandlers(int iKey, int iMask);
    void buttonSample(int nr, bool state, const ZGameTime& time);
    void AnalogSample(int nr, float state, float raw, const ZGameTime& time);
    void AnalogMotionSample(int nr, float state, const ZGameTime& time);
    const ZGameTime GetUpdateTime() const;
    const bool GetActive() const;
    void SetUpdateTime(const ZGameTime& time);

protected:
    char m_name[32];
    ZGameTime(*m_dighist)[2];
    bool* m_pbDigital;
    bool* m_pbDigitalTrap;
    int m_digcount;
    CtrlInfo* m_diginf;
    int m_diginfcount;
    float* m_anamotion;
    float* m_analog;
    float* m_anaraw;
    int m_anacount;
    CtrlInfo* m_anainf;
    int m_anainfcount;
    ZGameTime m_updatetime;
    bool m_active;
    bool m_connected;
    unsigned char m_iDeviceID;
    int(*m_pHandler[2])(int, int, void*);
    void* m_pHandlerData[2];
    int m_nExclusiveIndex;
    IInputDevice::EDeviceType m_type;
};

static_assert(sizeof(ZInputDevice) == 0x80);
static_assert(alignof(ZInputDevice) == 0x8);
