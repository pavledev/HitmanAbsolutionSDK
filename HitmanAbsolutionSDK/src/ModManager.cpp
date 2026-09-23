#include "ModManager.h"

#include <Windows.h>
#include <filesystem>

#include <ini.h>

#include "EventDispatcherImpl.h"
#include "HookImpl.h"
#include "IModInterface.h"
#include "Logging.h"
#include "Utils/StringUtils.h"
#include "UI/ModSelector.h"
#include "ModSDK.h"

static HMODULE LoadLibrarySilent(LPCSTR p_Path)
{
    // Prevent LoadLibrary from showing "entry point could not be located"
    // and similar errors when trying to load a mod that's no longer compatible
    // with the SDK.
    DWORD dwMode = SetErrorMode(SEM_FAILCRITICALERRORS);
    SetErrorMode(dwMode | SEM_FAILCRITICALERRORS);
    const auto module = LoadLibraryA(p_Path);
    SetErrorMode(dwMode);

    return module;
}

ModManager::ModManager() {}

ModManager::~ModManager()
{
    UnloadAllMods();
}

void ModManager::FindAvailableMods()
{
    m_IncompatibleMods.clear();
    m_AvailableMods.clear();
    m_AvailableModsLower.clear();

    // Discover and load mods.
    char exePathStr[MAX_PATH];
    auto pathSize = GetModuleFileNameA(nullptr, exePathStr, MAX_PATH);

    if (pathSize == 0)
    {
        return;
    }

    std::filesystem::path exePath(exePathStr);
    auto exeDir = exePath.parent_path();

    const auto modPath = absolute(exeDir / "mods");

    if (exists(modPath) && is_directory(modPath))
    {
        Logger::Debug("Looking for mods in '{}'...", modPath.string());

        for (const auto& entry : std::filesystem::directory_iterator(modPath))
        {
            if (!entry.is_regular_file())
            {
                continue;
            }

            if (entry.path().extension() != ".dll")
            {
                continue;
            }

            const auto name = entry.path().filename().stem().string();

            Logger::Debug("Found mod '{}' dll at: {}", name, entry.path().string());
            const auto module = LoadLibrarySilent(entry.path().string().c_str());

            if (module == nullptr)
            {
                Logger::Warn("Failed to load mod '{}' for availability check. Error: {}", entry.path().string(), GetLastError());
                m_IncompatibleMods.insert(name);
                continue;
            }

            const auto compiledSdkAbiVersionAddr = GetProcAddress(module, "CompiledSdkAbiVersion");

            if (compiledSdkAbiVersionAddr == nullptr)
            {
                FreeLibrary(module);
                Logger::Warn("Mod '{}' didn't export CompiledSdkAbiVersion. Marking as incompatible.", name);
                m_IncompatibleMods.insert(name);
                continue;
            }

            const auto compiledSdkAbiVersionFn = reinterpret_cast<CompiledSdkAbiVersion_t>(compiledSdkAbiVersionAddr);
            const int compiledSdkAbiVersion = compiledSdkAbiVersionFn();
            FreeLibrary(module);

            if (compiledSdkAbiVersion != HMASDK_ABI_VERSION)
            {
                Logger::Warn(
                    "Mod '{}' is compiled with ABI v{}, but current SDK is on ABI v{}. Marking as incompatible.", name, compiledSdkAbiVersion,
                    HMASDK_ABI_VERSION
                );
                m_IncompatibleMods.insert(name);
            }
            else
            {
                m_AvailableMods.insert(name);
                m_AvailableModsLower.insert(util::ToLowerCase(name));
            }
        }
    }
    else
    {
        Logger::Warn("Mod directory '{}' not found.", modPath.string());
    }

    ModSDK::GetInstance().GetUIModSelector()->UpdateAvailableMods(m_AvailableMods, m_IncompatibleMods, GetActiveMods());
}

std::unordered_set<std::string> ModManager::GetActiveMods()
{
    std::unordered_set<std::string> mods;

    LockRead();

    for (auto& loadedMod : m_LoadedMods)
    {
        mods.insert(loadedMod.first);
    }

    UnlockRead();

    return mods;
}

void ModManager::SetActiveMods(const std::unordered_set<std::string>& p_Mods)
{
    std::unordered_set<std::string> lowerModNames;

    for (auto& mod : p_Mods)
    {
        lowerModNames.insert(util::ToLowerCase(mod));
    }

    // First unload any mods that don't exist in the new list.
    std::vector<std::string> modsToUnload;

    LockRead();

    for (auto& pair : m_LoadedMods)
    {
        if (!lowerModNames.contains(pair.first))
        {
            modsToUnload.push_back(pair.first);
        }
    }

    UnlockRead();

    for (auto& mod : modsToUnload)
    {
        UnloadMod(mod);
    }

    std::vector<std::string> modsToLoad;

    LockRead();

    // Then load any mods that aren't already loaded.
    for (auto& mod : lowerModNames)
    {
        // Mod is already loaded; skip.
        if (m_LoadedMods.contains(mod))
        {
            continue;
        }

        modsToLoad.push_back(mod);
    }

    UnlockRead();

    for (auto& mod : modsToLoad)
    {
        LoadMod(mod, true);
    }

    // And persist the mods to the ini file.
    char exePathStr[MAX_PATH];
    auto pathSize = GetModuleFileNameA(nullptr, exePathStr, MAX_PATH);

    if (pathSize == 0)
    {
        return;
    }

    std::filesystem::path exePath(exePathStr);
    auto exeDir = exePath.parent_path();

    const auto iniPath = absolute(exeDir / "mods.ini");

    mINI::INIFile file(iniPath.string());

    mINI::INIStructure ini;

    if (is_regular_file(iniPath))
    {
        mINI::INIStructure oldIni;
        file.read(oldIni);

        if (oldIni.has("sdk"))
        {
            ini.set("sdk", oldIni.get("sdk"));
        }
    }

    for (auto& mod : p_Mods)
    {
        mINI::INIMap<std::string> emptyMap;
        ini.set(mod, emptyMap);
    }

    file.generate(ini, true);

    ModSDK::GetInstance().GetUIModSelector()->UpdateAvailableMods(m_AvailableMods, m_IncompatibleMods, GetActiveMods());
}

std::unordered_set<std::string> ModManager::GetAvailableMods()
{
    return m_AvailableMods;
}

void ModManager::LoadAllMods()
{
    FindAvailableMods();

    // Get the mods we want to load.
    char exePathStr[MAX_PATH];
    auto pathSize = GetModuleFileNameA(nullptr, exePathStr, MAX_PATH);

    if (pathSize == 0)
    {
        return;
    }

    std::filesystem::path exePath(exePathStr);
    auto exeDir = exePath.parent_path();

    const auto iniPath = absolute(exeDir / "mods.ini");

    mINI::INIFile file(iniPath.string());
    mINI::INIStructure ini;

    // now we can read the file
    file.read(ini);

    for (auto& mod : ini)
    {
        // Ignore the SDK entry. It's used for configuring the SDK itself.
        if (util::ToLowerCase(mod.first) == "sdk")
        {
            continue;
        }

        if (m_AvailableModsLower.contains(util::ToLowerCase(mod.first)))
        {
            LoadMod(mod.first, false);
        }
    }

    ModSDK::GetInstance().GetUIModSelector()->UpdateAvailableMods(m_AvailableMods, m_IncompatibleMods, GetActiveMods());
}

void ModManager::LoadMod(const std::string& p_Name, bool p_LiveLoad)
{
    const std::string name = util::ToLowerCase(p_Name);
    IModInterface* modInterface;

    {
        std::unique_lock lock(m_Mutex);

        if (m_LoadedMods.contains(name))
        {
            Logger::Warn("A mod with the same name ({}) is already loaded. Skipping.", p_Name);
            return;
        }

        char exePathStr[MAX_PATH];
        auto pathSize = GetModuleFileNameA(nullptr, exePathStr, MAX_PATH);

        if (pathSize == 0)
        {
            return;
        }

        std::filesystem::path exePath(exePathStr);
        auto exeDir = exePath.parent_path();

        const auto modulePath = absolute(exeDir / ("mods/" + p_Name + ".dll"));

        if (!exists(modulePath) || !is_regular_file(modulePath))
        {
            Logger::Warn("Could not find mod '{}'.", p_Name);
        }

        Logger::Info("Attempting to load mod '{}'.", p_Name);
        Logger::Debug("Module path is '{}'.", modulePath.string());

        const auto module = LoadLibrarySilent(modulePath.string().c_str());

        if (module == nullptr)
        {
            Logger::Warn("Failed to load mod. Error: {}", GetLastError());
            return;
        }

        const auto getModInterfaceAddr = GetProcAddress(module, "GetModInterface");

        if (getModInterfaceAddr == nullptr)
        {
            Logger::Warn("Could not find mod interface for mod. Make sure that the 'GetModInterface' method is exported.");
            FreeLibrary(module);
            return;
        }

        const auto getModInterface = reinterpret_cast<GetModInterface_t>(getModInterfaceAddr);

        const auto compiledSdkAbiVersionAddr = GetProcAddress(module, "CompiledSdkAbiVersion");

        if (compiledSdkAbiVersionAddr == nullptr)
        {
            Logger::Warn("Could not find CompiledSdkAbiVersion export in mod '{}'. This probably means the mod has not been updated!", name);
            FreeLibrary(module);
            return;
        }

        const auto compiledSdkAbiVersionFn = reinterpret_cast<CompiledSdkAbiVersion_t>(compiledSdkAbiVersionAddr);
        const int compiledSdkAbiVersion = compiledSdkAbiVersionFn();

        if (compiledSdkAbiVersion != HMASDK_ABI_VERSION)
        {
            Logger::Warn(
                "Mod '{}' is compiled with ABI v{}, but current SDK is on ABI v{}. This probably means the mod has not been updated!", name,
                compiledSdkAbiVersion, HMASDK_ABI_VERSION
            );
            FreeLibrary(module);
            return;
        }

        modInterface = getModInterface();

        if (modInterface == nullptr)
        {
            Logger::Warn("Mod returned a null mod interface.");
            FreeLibrary(module);
            return;
        }

        LoadedMod mod{};
        mod.m_Module = module;
        mod.m_ModInterface = modInterface;
        mod.m_Settings = new ModSettings(p_Name, exeDir / "mods");

        m_LoadedMods[name] = mod;
        m_ModList.push_back(modInterface);
    }

    ModSDK::GetInstance().OnModLoaded(name, modInterface, p_LiveLoad);
}

void ModManager::UnloadMod(const std::string& p_Name)
{
    std::unique_lock lock(m_Mutex);

    const std::string name = util::ToLowerCase(p_Name);

    auto modMapIt = m_LoadedMods.find(name);

    if (modMapIt == m_LoadedMods.end())
    {
        return;
    }

    Logger::Info("Unloading mod '{}'.", p_Name);

    HookRegistry::ClearDetoursWithContext(modMapIt->second.m_ModInterface);
    EventDispatcherRegistry::ClearModListeners(modMapIt->second.m_ModInterface);

    for (auto it = m_ModList.begin(); it != m_ModList.end();)
    {
        if (*it == modMapIt->second.m_ModInterface)
        {
            it = m_ModList.erase(it);
        }
        else
        {
            ++it;
        }
    }

    ModSDK::GetInstance().OnModUnloading(name, modMapIt->second.m_ModInterface);

    delete modMapIt->second.m_ModInterface;
    delete modMapIt->second.m_Settings;
    FreeLibrary(modMapIt->second.m_Module);

    m_LoadedMods.erase(modMapIt);

    ModSDK::GetInstance().OnModUnloaded(name);
}

void ModManager::ReloadMod(const std::string& p_Name)
{
    LockRead();

    const std::string name = util::ToLowerCase(p_Name);

    auto it = m_LoadedMods.find(name);

    if (it == m_LoadedMods.end())
    {
        Logger::Warn("Could not find mod '{}' to reload.", p_Name);
        UnlockRead();
        return;
    }

    UnlockRead();

    UnloadMod(p_Name);
    LoadMod(p_Name, true);
}

void ModManager::UnloadAllMods()
{
    LockRead();

    std::vector<std::string> modNames;

    for (auto& pair : m_LoadedMods)
    {
        modNames.push_back(pair.first);
    }

    UnlockRead();

    for (auto& mod : modNames)
    {
        UnloadMod(mod);
    }
}

void ModManager::ReloadAllMods()
{
    LockRead();

    std::vector<std::string> modNames;

    for (auto& pair : m_LoadedMods)
    {
        modNames.push_back(pair.first);
    }

    UnlockRead();

    for (auto& mod : modNames)
    {
        ReloadMod(mod);
    }
}

IModInterface* ModManager::GetModByName(const std::string& p_Name)
{
    std::shared_lock lock(m_Mutex);

    std::string name = util::ToLowerCase(p_Name);

    auto it = m_LoadedMods.find(name);

    if (it == m_LoadedMods.end())
    {
        return nullptr;
    }

    return it->second.m_ModInterface;
}

ModSettings* ModManager::GetModSettings(IModInterface* p_ModInterface)
{
    std::shared_lock lock(m_Mutex);

    for (auto& pair : m_LoadedMods)
    {
        if (pair.second.m_ModInterface == p_ModInterface)
        {
            return pair.second.m_Settings;
        }
    }

    return nullptr;
}
