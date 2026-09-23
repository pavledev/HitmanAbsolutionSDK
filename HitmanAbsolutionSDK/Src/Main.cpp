#include "ModSDK.h"

DWORD WINAPI StartupProc(LPVOID)
{
    ModSDK::GetInstance().Startup();

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        HANDLE handle = CreateThread(nullptr, 0, StartupProc, nullptr, 0, nullptr);

        if (handle)
        {
            CloseHandle(handle);
        }
    }

    return TRUE;
}
