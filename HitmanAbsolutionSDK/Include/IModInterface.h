#pragma once

#include "Logging.h"
#include "IModSDK.h"
#include "IImGuiRenderer.h"
#include "IDirectXRenderer.h"
#include "Hooks.h"

class IModInterface
{
  public:
    virtual ~IModInterface() = default;

  private:
    virtual void SetupUI(IImGuiRenderer* p_Renderer)
    {
        if (!p_Renderer)
        {
            return;
        }

        auto* context = p_Renderer->GetContext();

        if (!context)
        {
            return;
        }

        ImGui::SetCurrentContext(context);
        ImGui::SetAllocatorFunctions(p_Renderer->GetMemAlloc(), p_Renderer->GetMemFree(), p_Renderer->GetAllocatorUserData());

#ifdef HMASDK_HAS_IMPLOT
        ImPlot::SetCurrentContext(p_Renderer->GetImPlotContext());
#endif
    }

  public:
    // Invoked on the game thread.
    virtual void Initialize() {}

    // Invoked on the game thread.
    virtual void OnEngineInitialized() {}

    // Invoked on the render thread.
    virtual void OnDrawUI(IImGuiRenderer* p_Renderer, bool p_HasFocus) {}

    virtual void OnDrawMenu(IImGuiRenderer* p_Renderer) {}

    // Invoked on the render thread.
    virtual void OnDraw3D(IDirectXRenderer* p_Renderer) {}

    /**
     * Draw with a depth buffer active, so drawn objects can be occluded by objects in the scene.
     *
     * Invoked on the render thread.
     */
    virtual void OnDepthDraw3D(IDirectXRenderer* p_Renderer) {}

    // Invoked on the render thread.
    virtual void OnDraw2D(IDirectXRenderer* p_Renderer) {}

  private:
    virtual void CleanupUI()
    {
        ImGui::SetCurrentContext(nullptr);

#ifdef HMASDK_HAS_IMPLOT
        ImPlot::SetCurrentContext(nullptr);
#endif
    }

  public:
    /**
     * Set a setting string value for the given name.
     * @param p_Section The section of the setting in the INI file.
     * @param p_Name The name of the setting.
     * @param p_Value The value of the setting.
     */
    void SetSetting(const ZString& p_Section, const ZString& p_Name, const ZString& p_Value)
    {
        SDK().SetModSetting(this, p_Section, p_Name, p_Value);
    }

    /**
     * Set a setting integer value for the given name.
     * @param p_Section The section of the setting in the INI file.
     * @param p_Name The name of the setting.
     * @param p_Value The value of the setting.
     */
    void SetSettingInt(const ZString& p_Section, const ZString& p_Name, int64_t p_Value)
    {
        SDK().SetModSettingInt(this, p_Section, p_Name, p_Value);
    }

    /**
     * Set a setting unsigned integer value for the given name.
     * @param p_Section The section of the setting in the INI file.
     * @param p_Name The name of the setting.
     * @param p_Value The value of the setting.
     */
    void SetSettingUInt(const ZString& p_Section, const ZString& p_Name, uint64_t p_Value)
    {
        SDK().SetModSettingUInt(this, p_Section, p_Name, p_Value);
    }

    /**
     * Set a setting double value for the given name.
     * @param p_Section The section of the setting in the INI file.
     * @param p_Name The name of the setting.
     * @param p_Value The value of the setting.
     */
    void SetSettingDouble(const ZString& p_Section, const ZString& p_Name, double p_Value)
    {
        SDK().SetModSettingDouble(this, p_Section, p_Name, p_Value);
    }

    /**
     * Set a setting boolean value for the given name.
     * @param p_Section The section of the setting in the INI file.
     * @param p_Name The name of the setting.
     * @param p_Value The value of the setting.
     */
    void SetSettingBool(const ZString& p_Section, const ZString& p_Name, bool p_Value)
    {
        SDK().SetModSettingBool(this, p_Section, p_Name, p_Value);
    }

    /**
     * Get a setting string value for the given name.
     * @param p_Section The section of the setting in the INI file.
     * @param p_Name The name of the setting.
     * @param p_DefaultValue The default value to use if the setting does not exist.
     * @param p_OutValue Receives the value of the setting, or p_DefaultValue if the setting does not exist.
     */
    void GetSetting(const ZString& p_Section, const ZString& p_Name, const ZString& p_DefaultValue, ZString& p_OutValue)
    {
        SDK().GetModSetting(this, p_Section, p_Name, p_DefaultValue, p_OutValue);
    }

    /**
     * Get a setting integer value for the given name.
     * @param p_Section The section of the setting in the INI file.
     * @param p_Name The name of the setting.
     * @param p_DefaultValue The default value to return if the setting does not exist or is not an integer.
     * @return The value of the setting, or the default value if the setting does not exist or is not an integer.
     */
    int64_t GetSettingInt(const ZString& p_Section, const ZString& p_Name, int64_t p_DefaultValue)
    {
        return SDK().GetModSettingInt(this, p_Section, p_Name, p_DefaultValue);
    }

    /**
     * Get a setting unsigned integer value for the given name.
     * @param p_Section The section of the setting in the INI file.
     * @param p_Name The name of the setting.
     * @param p_DefaultValue The default value to return if the setting does not exist or is not an unsigned integer.
     * @return The value of the setting, or the default value if the setting does not exist or is not an unsigned integer.
     */
    uint64_t GetSettingUInt(const ZString& p_Section, const ZString& p_Name, uint64_t p_DefaultValue)
    {
        return SDK().GetModSettingUInt(this, p_Section, p_Name, p_DefaultValue);
    }

    /**
     * Get a setting double value for the given name.
     * @param p_Section The section of the setting in the INI file.
     * @param p_Name The name of the setting.
     * @param p_DefaultValue The default value to return if the setting does not exist or is not a double.
     * @return The value of the setting, or the default value if the setting does not exist or is not a double.
     */
    double GetSettingDouble(const ZString& p_Section, const ZString& p_Name, double p_DefaultValue)
    {
        return SDK().GetModSettingDouble(this, p_Section, p_Name, p_DefaultValue);
    }

    /**
     * Get a setting boolean value for the given name.
     * @param p_Section The section of the setting in the INI file.
     * @param p_Name The name of the setting.
     * @param p_DefaultValue The default value to return if the setting does not exist or is not a boolean.
     * @return The value of the setting, or the default value if the setting does not exist or is not a boolean.
     */
    bool GetSettingBool(const ZString& p_Section, const ZString& p_Name, bool p_DefaultValue)
    {
        return SDK().GetModSettingBool(this, p_Section, p_Name, p_DefaultValue);
    }

    /**
     * Check if a setting with the given name exists.
     * @param p_Section The section of the setting in the INI file.
     * @param p_Name The name of the setting.
     * @return True if the setting exists, false otherwise.
     */
    bool HasSetting(const ZString& p_Section, const ZString& p_Name)
    {
        return SDK().HasModSetting(this, p_Section, p_Name);
    }

    /**
     * Remove a setting with the given name.
     * @param p_Section The section of the setting in the INI file.
     * @param p_Name The name of the setting.
     */
    void RemoveSetting(const ZString& p_Section, const ZString& p_Name)
    {
        SDK().RemoveModSetting(this, p_Section, p_Name);
    }

    /**
     * Reload the settings for the current mod.
     */
    void ReloadSettings()
    {
        SDK().ReloadModSettings(this);
    }

    friend class ModSDK;
};

typedef IModInterface*(__cdecl* GetModInterface_t)();
typedef int(__cdecl* CompiledSdkAbiVersion_t)();

#define DECLARE_HMASDK_MOD(ModClass)                                   \
    extern "C" __declspec(dllexport) IModInterface* GetModInterface(); \
    extern "C" __declspec(dllexport) const char* CompiledSdkVersion(); \
    extern "C" __declspec(dllexport) int CompiledSdkAbiVersion();      \
                                                                       \
    inline ModClass* Mod()                                             \
    {                                                                  \
        return reinterpret_cast<ModClass*>(GetModInterface());         \
    }

/**
 * Note that `ModClass` cannot be in a namespace (ie: `DEFINE_HMASDK_MOD(MyMod::Mod)` will fail to compile).
 */
#define DEFINE_HMASDK_MOD(ModClass)                                   \
    static IModInterface* g_##ModClass##_Instance = nullptr;          \
                                                                      \
    extern "C" __declspec(dllexport) IModInterface* GetModInterface() \
    {                                                                 \
        if (g_##ModClass##_Instance == nullptr)                       \
            g_##ModClass##_Instance = new ModClass();                 \
                                                                      \
        return g_##ModClass##_Instance;                               \
    }                                                                 \
                                                                      \
    extern "C" __declspec(dllexport) const char* CompiledSdkVersion() \
    {                                                                 \
        return HMASDK_VERSION;                                        \
    }                                                                 \
    extern "C" __declspec(dllexport) int CompiledSdkAbiVersion()      \
    {                                                                 \
        return HMASDK_ABI_VERSION;                                    \
    }

#define DECLARE_CDECL_MOD_DETOUR(ModClass, ReturnType, DetourName, ...) \
    DECLARE_CDECL_DETOUR_WITH_CONTEXT(ModClass, ReturnType, DetourName, __VA_ARGS__)

#define DECLARE_STDCALL_MOD_DETOUR(ModClass, ReturnType, DetourName, ...) \
    DECLARE_STDCALL_DETOUR_WITH_CONTEXT(ModClass, ReturnType, DetourName, __VA_ARGS__)

#define DECLARE_FASTCALL_MOD_DETOUR(ModClass, ReturnType, DetourName, ...) \
    DECLARE_FASTCALL_DETOUR_WITH_CONTEXT(ModClass, ReturnType, DetourName, __VA_ARGS__)

#define DECLARE_THISCALL_MOD_DETOUR(ModClass, ReturnType, DetourName, ...) \
    DECLARE_THISCALL_DETOUR_WITH_CONTEXT(ModClass, ReturnType, DetourName, __VA_ARGS__)

#define DEFINE_CDECL_MOD_DETOUR(ModClass, ReturnType, DetourName, ...) DEFINE_CDECL_DETOUR_WITH_CONTEXT(ModClass, ReturnType, DetourName, __VA_ARGS__)

#define DEFINE_STDCALL_MOD_DETOUR(ModClass, ReturnType, DetourName, ...) \
    DEFINE_STDCALL_DETOUR_WITH_CONTEXT(ModClass, ReturnType, DetourName, __VA_ARGS__)

#define DEFINE_FASTCALL_MOD_DETOUR(ModClass, ReturnType, DetourName, ...) \
    DEFINE_FASTCALL_DETOUR_WITH_CONTEXT(ModClass, ReturnType, DetourName, __VA_ARGS__)

#define DEFINE_THISCALL_MOD_DETOUR(ModClass, ReturnType, DetourName, ...) \
    DEFINE_THISCALL_DETOUR_WITH_CONTEXT(ModClass, ReturnType, DetourName, __VA_ARGS__)

#define DECLARE_MOD_LISTENER(ModClass, EventName, ...)                           \
    template<class... Args> static void EventName(void* th, Args... p_Args)      \
    {                                                                            \
        return reinterpret_cast<ModClass*>(th)->EventName##_Internal(p_Args...); \
    }                                                                            \
                                                                                 \
    void EventName##_Internal(__VA_ARGS__);

#define DEFINE_MOD_LISTENER(ModClass, EventName, ...) void ModClass::EventName##_Internal(__VA_ARGS__)
