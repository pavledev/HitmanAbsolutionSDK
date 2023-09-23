#include "dinput.h"

static HMODULE originalDirectInput = nullptr;
static HMODULE sdk = nullptr;

using DirectInput8Create_t = HRESULT(WINAPI*)(HINSTANCE, DWORD, REFIID, LPVOID*, LPUNKNOWN);
static DirectInput8Create_t originalDirectInput8Create = nullptr;

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        char dinput8Path[MAX_PATH] = { 0 };

        GetSystemDirectoryA(dinput8Path, MAX_PATH);
        strcat_s(dinput8Path, MAX_PATH, "\\dinput8.dll");

        originalDirectInput = LoadLibraryA(dinput8Path);

        if (!originalDirectInput)
        {
            return false;
        }

        originalDirectInput8Create = reinterpret_cast<DirectInput8Create_t>(GetProcAddress(originalDirectInput, "DirectInput8Create"));

        if (!originalDirectInput8Create)
        {
            return false;
        }

        sdk = LoadLibraryA("HitmanAbsolutionSDK");

        if (!sdk)
        {
            return false;
        }
    }
    else if (fdwReason == DLL_PROCESS_DETACH)
    {
        if (originalDirectInput)
        {
            FreeLibrary(originalDirectInput);

            originalDirectInput = nullptr;
            originalDirectInput8Create = nullptr;
        }

        if (sdk)
        {
            FreeLibrary(sdk);

            sdk = nullptr;
        }
    }

    return true;
}

HRESULT WINAPI DirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID* ppvOut, LPUNKNOWN punkOuter)
{
    if (originalDirectInput8Create)
    {
        return originalDirectInput8Create(hinst, dwVersion, riidltf, ppvOut, punkOuter);
    }

    return E_FAIL;
}
