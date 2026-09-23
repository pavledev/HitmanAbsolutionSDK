#pragma once

#include <span>

#include <spdlog/spdlog.h>

#include "Common.h"
#include "ModSDKVersion.h"

class ZString;
struct SVector2;
struct SVector3;
struct SMatrix;
class ZDynamicResourceLibrary;
class ZRuntimeResourceID;

class IModInterface;

class IModSDK
{
  public:
    virtual void Log(spdlog::level::level_enum p_Level, std::string_view p_Msg) = 0;

    /**
     * Search for a pattern in the game's memory and patch it with the given code.
     * @param p_Pattern A sequence of bytes to
     * search for in the game's memory.
     * @param p_Mask A mask to use when searching for the pattern. x = pattern byte, ? = any byte (eg.
     * xxx????x).
     * @param p_NewCode A buffer containing the new code to write to the location where the pattern was found.
     *
     * @param p_CodeSize The size of the code buffer.
     * @param p_Offset The offset to add to the address where the pattern was found.

     * @return True if the pattern was found and patched, false otherwise.
     */
    virtual bool PatchCode(const char* p_Pattern, const char* p_Mask, void* p_NewCode, size_t p_CodeSize, ptrdiff_t p_Offset) = 0;

    /**
     * Search for a pattern in the game's memory and patch it with the given code, storing the original code in a provided buffer.
     * @param p_Pattern A sequence of bytes to search for in the game's memory.
     * @param p_Mask A mask to use when searching for the
     * pattern. x = pattern byte, ? = any byte (eg. xxx????x).
     * @param p_NewCode A buffer containing the new code to write to the
     * location where the pattern was found.
     * @param p_CodeSize The size of the code buffer.
     * @param p_Offset The offset to
     * add to the address where the pattern was found.
     * @param p_OriginalCode A buffer to store the original code.
     * @return
     * True if the pattern was found and patched, false otherwise.
     */
    virtual bool PatchCodeStoreOriginal(
        const char* p_Pattern, const char* p_Mask, void* p_NewCode, size_t p_CodeSize, ptrdiff_t p_Offset, void* p_OriginalCode
    ) = 0;

    /**
     * Get a mod setting string value for the given name.
     * @param p_Mod The mod to get the setting for.
     * @param p_Section The section of the setting in the INI file.
     * @param p_Name The name of the setting.
     * @param p_DefaultValue The default value to use if the setting does not exist.
     * @param p_OutValue Receives the value of the setting, or p_DefaultValue if the setting does not exist.
     */
    virtual void
    GetModSetting(IModInterface* p_Mod, const ZString& p_Section, const ZString& p_Name, const ZString& p_DefaultValue, ZString& p_OutValue) = 0;

    /**
     * Get a mod setting integer value for the given name.
     * @param p_Mod The mod to get the setting for.
     * @param p_Section The section of the setting in the INI file.
     * @param p_Name The name of the setting.
     * @param p_DefaultValue The default value to return if the setting does not exist or is not an integer.
     * @return The value of the setting, or the default value if the setting does not exist or is not an integer.
     */
    virtual int64_t GetModSettingInt(IModInterface* p_Mod, const ZString& p_Section, const ZString& p_Name, int64_t p_DefaultValue) = 0;

    /**
     * Get a mod setting unsigned integer value for the given name.
     * @param p_Mod The mod to get the setting for.
     * @param p_Section The section of the setting in the INI file.
     * @param p_Name The name of the setting.
     * @param p_DefaultValue The default value to return if the setting does not exist or is not an unsigned integer.
     * @return The value of the setting, or the default value if the setting does not exist or is not an unsigned integer.
     */
    virtual uint64_t GetModSettingUInt(IModInterface* p_Mod, const ZString& p_Section, const ZString& p_Name, uint64_t p_DefaultValue) = 0;

    /**
     * Get a mod setting double value for the given name.
     * @param p_Mod The mod to get the setting for.
     * @param p_Section The section of the setting in the INI file.
     * @param p_Name The name of the setting.
     * @param p_DefaultValue The default value to return if the setting does not exist or is not a double.
     * @return The value of the setting, or the default value if the setting does not exist or is not a double.
     */
    virtual double GetModSettingDouble(IModInterface* p_Mod, const ZString& p_Section, const ZString& p_Name, double p_DefaultValue) = 0;

    /**
     * Get a mod setting boolean value for the given name.
     * @param p_Mod The mod to get the setting for.
     * @param p_Section The section of the setting in the INI file.
     * @param p_Name The name of the setting.
     * @param p_DefaultValue The default value to return if the setting does not exist or is not a boolean.
     * @return The value of the setting, or the default value if the setting does not exist or is not a boolean.
     */
    virtual bool GetModSettingBool(IModInterface* p_Mod, const ZString& p_Section, const ZString& p_Name, bool p_DefaultValue) = 0;

    /**
     * Set a mod setting value for the given name.
     * @param p_Mod The mod to set the setting for.
     * @param p_Section The section of the setting in the INI file.
     * @param p_Name The name of the setting.
     * @param p_Value The value of the setting.
     */
    virtual void SetModSetting(IModInterface* p_Mod, const ZString& p_Section, const ZString& p_Name, const ZString& p_Value) = 0;

    /**
     * Set a mod setting integer value for the given name.
     * @param p_Mod The mod to set the setting for.
     * @param p_Section The section of the setting in the INI file.
     * @param p_Name The name of the setting.
     * @param p_Value The value of the setting.
     */
    virtual void SetModSettingInt(IModInterface* p_Mod, const ZString& p_Section, const ZString& p_Name, int64_t p_Value) = 0;

    /**
     * Set a mod setting unsigned integer value for the given name.
     * @param p_Mod The mod to set the setting for.
     * @param p_Section The section of the setting in the INI file.
     * @param p_Name The name of the setting.
     * @param p_Value The value of the setting.
     */
    virtual void SetModSettingUInt(IModInterface* p_Mod, const ZString& p_Section, const ZString& p_Name, uint64_t p_Value) = 0;

    /**
     * Set a mod setting double value for the given name.
     * @param p_Mod The mod to set the setting for.
     * @param p_Section The section of the setting in the INI file.
     * @param p_Name The name of the setting.
     * @param p_Value The value of the setting.
     */
    virtual void SetModSettingDouble(IModInterface* p_Mod, const ZString& p_Section, const ZString& p_Name, double p_Value) = 0;

    /**
     * Set a mod setting boolean value for the given name.
     * @param p_Mod The mod to set the setting for.
     * @param p_Section The section of the setting in the INI file.
     * @param p_Name The name of the setting.
     * @param p_Value The value of the setting.
     */
    virtual void SetModSettingBool(IModInterface* p_Mod, const ZString& p_Section, const ZString& p_Name, bool p_Value) = 0;

    /**
     * Check if a mod setting with the given name exists.
     * @param p_Mod The mod to check the setting for.
     * @param p_Section The section of the setting in the INI file.
     * @param p_Name The name of the setting.
     * @return True if the setting exists, false otherwise.
     */
    virtual bool HasModSetting(IModInterface* p_Mod, const ZString& p_Section, const ZString& p_Name) = 0;

    /**
     * Remove a mod setting with the given name.
     * @param p_Mod The mod to remove the setting for.
     * @param p_Section The section of the setting in the INI file.
     * @param p_Name The name of the setting.
     */
    virtual void RemoveModSetting(IModInterface* p_Mod, const ZString& p_Section, const ZString& p_Name) = 0;

    /**
     * Reload the settings for the given mod.
     * @param p_Mod The mod to reload the settings for.
     */
    virtual void ReloadModSettings(IModInterface* p_Mod) = 0;

    /**
     * Convert a position in 3D world space to a 2D position on the screen.
     * @param p_WorldPos The 3D world position.
     * @param p_Out The output 2D position on the screen.
     * @return True if the conversion was successful, false otherwise (eg. if the position is behind the camera).
     */
    virtual bool WorldToScreen(const SVector3& p_WorldPos, SVector2& p_Out) = 0;

    /**
     * Convert a 2D position on the screen to a 3D world position.
     * @param p_ScreenPos The 2D position on the screen.
     * @param p_OutWorldPos The output 3D world position.
     * @param p_OutDirection The output direction of the ray.
     * @return True if the conversion was successful, false otherwise (eg. if the position is outside the screen bounds).
     */
    virtual bool ScreenToWorld(const SVector2& p_ScreenPos, SVector3& p_OutWorldPos, SVector3& p_OutDirection) = 0;

    /**
     * Give focus to the SDK UI.
     * This prevents the user from interacting with the game
     * and allows the SDK UI to receive input.
     */
    virtual void RequestUIFocus() = 0;

    /**
     * Release focus from the SDK UI.
     * This allows the user to interact with the game again.
     */
    virtual void ReleaseUIFocus() = 0;

    /**
     * Get the resource ID associated with a runtime resource ID.
     * @param p_RuntimeResourceID The runtime resource ID.
     * @return The resource ID, or nullptr if it was not found.
     */
    virtual const char* GetResourceID(uint64_t p_RuntimeResourceID) const = 0;

    /**
     * Get the runtime resource ID associated with a resource ID.
     * If the resource ID is not registered, its runtime resource ID is generated from the resource ID.
     * @param p_ResourceID The resource ID.
     * @return The runtime resource ID.
     */
    virtual uint64_t GetRuntimeResourceID(const std::string& p_ResourceID) const = 0;

    /**
     * Get the values of an enum type.
     * @param p_TypeName The name of the enum type.
     * @return The enum values mapped to their names, or an empty map if the enum type was not found.
     */
    virtual const std::map<int32_t, std::string>& GetEnum(const std::string& p_TypeName) const = 0;

    /**
     * Get the name associated with a property ID.
     * @param p_PropertyID The property ID.
     * @return The property name, or an empty string if the property was not found.
     */
    virtual const std::string& GetPropertyName(uint32_t p_PropertyID) const = 0;

    /**
     * Get an embedded binary resource.
     * @param p_ResourceID The ID of the embedded resource.
     * @return A span containing the resource data.
     */
    virtual std::span<const std::byte> GetResource(int32_t p_ResourceID) const = 0;

    /**
     * Get an embedded text resource.
     * @param p_ResourceID The ID of the embedded resource.
     * @return A string view containing the resource data.
     */
    virtual std::string_view GetTextResource(int32_t p_ResourceID) const = 0;

    /**
     * Install a dynamic resource library using the specified header library.
     * @param p_HeaderLibraryRuntimeResourceID The runtime resource ID of the header library.
     * @param p_DynamicResourceLibrary Receives the installed dynamic resource library.
     * @param p_TempRuntimeResourceID Receives the runtime resource ID of the main template resource.
     * @param p_EntityCount The number of entities that will be spawned.
     * @return True if the dynamic resource library was installed successfully, false otherwise.
     */
    virtual bool InstallDynamicResourceLibrary(
        const ZRuntimeResourceID& p_HeaderLibraryRuntimeResourceID, ZDynamicResourceLibrary*& p_DynamicResourceLibrary,
        ZRuntimeResourceID& p_TempRuntimeResourceID, uint32_t p_EntityCount = 1
    ) const = 0;

    /**
     * Install a dynamic resource library using the specified header library.
     * @param p_HeaderLibraryResourceID The resource ID of the header library.
     * @param p_DynamicResourceLibrary Receives the installed dynamic resource library.
     * @param p_TempRuntimeResourceID Receives the runtime resource ID of the main template resource.
     * @param p_EntityCount The number of entities that will be spawned.
     * @return True if the dynamic resource library was installed successfully, false otherwise.
     */
    virtual bool InstallDynamicResourceLibrary(
        const std::string& p_HeaderLibraryResourceID, ZDynamicResourceLibrary*& p_DynamicResourceLibrary, ZRuntimeResourceID& p_TempRuntimeResourceID,
        const uint32_t p_EntityCount = 1
    ) const = 0;

    /**
     * Create and install a dynamic resource library for the specified resource.
     * @param p_ResourceID The resource ID to create the dynamic resource library for.
     * @param p_DynamicResourceLibrary Receives the installed dynamic resource library.
     * @param p_TempRuntimeResourceID Receives the runtime resource ID of the main template resource.
     * @param p_EntityCount The number of entities that will be spawned.
     * @return True if the dynamic resource library was created and installed successfully, false otherwise.
     */
    virtual bool CreateAndInstallDynamicResourceLibrary(
        const std::string& p_ResourceID, ZDynamicResourceLibrary*& p_DynamicResourceLibrary, ZRuntimeResourceID& p_TempRuntimeResourceID,
        const uint32_t p_EntityCount = 1
    ) = 0;

    virtual void AllocateZString(ZString* p_Target, const char* p_Str, uint32_t p_Size) = 0;
    virtual void FreeZString(ZString* p_Target) = 0;
};

HitmanAbsolutionSDK_API IModSDK& SDK();

extern "C" HitmanAbsolutionSDK_API const char* SDKVersion();
