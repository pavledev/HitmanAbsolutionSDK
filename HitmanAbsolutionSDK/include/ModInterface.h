#pragma once

class ModInterface
{
public:
    virtual ~ModInterface() = default;

    virtual void Initialize()
    {
    }

    virtual void OnEngineInitialized()
    {
    }

    virtual void OnDrawUI(bool hasFocus)
    {
    }

    virtual void OnDraw3D()
    {
    }

    virtual void OnOnDrawMenu()
    {
    }

    void SetupUI();
};

using GetModInterface_t = ModInterface * (*)();

#define DECLARE_MOD(ModClass) \
    extern "C" __declspec(dllexport) ModInterface* GetModInterface(); \
    \
    ModClass* GetInstance() \
    { \
        return static_cast<ModClass*>(GetPluginInterface()); \
    }

#define DEFINE_MOD(ModClass) \
    static ModInterface* ModClass ## Singleton = nullptr; \
    \
    extern "C" __declspec(dllexport) ModInterface* GetModInterface() \
    { \
        if (!ModClass ## Singleton) \
            ModClass ## Singleton = new ModClass(); \
        \
        return ModClass ## Singleton; \
    }
