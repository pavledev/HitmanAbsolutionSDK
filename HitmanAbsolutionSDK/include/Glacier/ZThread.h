#pragma once

#include <Windows.h>
#include <mmsystem.h>

#include "ZDelegate.h"
#include "ZString.h"

class ZThread
{
  public:
    static void Sleep(uint32_t p_Milliseconds)
    {
        if (p_Milliseconds >= 20)
        {
            ::Sleep(p_Milliseconds);
        }
        else
        {
            timeBeginPeriod(1);
            ::Sleep(p_Milliseconds);
            timeEndPeriod(1);
        }
    }

    ZDelegate<uint32_t __cdecl(void*)> m_function;
    int32_t m_handle;
    ZString m_sName;
    uint32_t m_nExitCode;
    int32_t m_nStackSize;
    uint8_t m_nJoinState;
};
