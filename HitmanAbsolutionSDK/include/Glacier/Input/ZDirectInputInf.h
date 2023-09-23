#pragma once

#define DIRECTINPUT_VERSION 0x0800

#include <dinput.h>

class ZDirectInputInf
{
public:
    IDirectInputDevice8A* GetDirectInputDevice() const;
    const bool GetAcquired() const;
    void SetAcquired(const bool acquired);

private:
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
