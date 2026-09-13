#include "SDK.h"

DWORD WINAPI MainThread(LPVOID)
{
    SDK& sdk = SDK::GetInstance();

    sdk.Setup();

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        HANDLE handle = CreateThread(nullptr, 0, MainThread, hModule, 0, nullptr);

        if (handle)
        {
            CloseHandle(handle);
        }
    }
    else if (dwReason == DLL_PROCESS_DETACH)
    {
        HMODULE module = GetModuleHandleA("HitmanAbsolutionSDK.dll");

        if (module)
        {
            FreeLibraryAndExitThread(module, 0);
        }
    }

    return TRUE;
}
