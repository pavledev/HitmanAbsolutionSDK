#pragma once

class ZGameTime;

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
    virtual int DigitalState(int param1) const = 0;
    virtual char* DigitalName(int param1) const = 0;
    virtual int DigitalId(const char* param1) const = 0;
    virtual ZGameTime DigitalHist(int param1, int param2) const = 0;
    virtual int AnalogCount() const = 0;
    virtual float AnalogState(int param1) const = 0;
    virtual float AnalogStateRaw(int param1) const = 0;
    virtual char* AnalogName(int param1) const = 0;
    virtual int AnalogId(const char* param1) const = 0;
    virtual float AnalogMotion(int param1) const = 0;
    virtual void ResetTables(bool param1) = 0;
    virtual void ClearKey(int param1) = 0;
    virtual bool HapticEnabled() = 0;
    virtual void RumbleSetPitch(int param1, float param2) = 0;
    virtual bool StartHaptic(unsigned int param1, float param2, float param3, bool param4) = 0;
    virtual void StopHaptics() = 0;
    virtual void SetRumbleBGPitch(float param1, float param2) = 0;
    virtual void SetRumblePitchDirect(float param1, float param2) = 0;
    virtual void Activate() = 0;
    virtual void Deactivate() = 0;
    virtual char* GetName() = 0;
    virtual void PrintInfo() = 0;
    virtual bool Connected() = 0;
};
