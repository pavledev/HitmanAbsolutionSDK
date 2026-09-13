#include <filesystem>

#define MINI_CASE_SENSITIVE

#include <ini.h>

#include "ModManager.h"
#include "Logging.h"
#include "SDK.h"
#include "IModInterface.h"
#include "UI/ModSelector.h"

ModManager::ModManager()
{
    InitializeSRWLock(&m_SrwLock);
}

ModManager::~ModManager()
{
    UnloadAllMods();
}

std::set<std::string> ModManager::GetActiveMods()
{
    ScopedSharedGuard scopedSharedGuard = ScopedSharedGuard(&m_SrwLock);
    std::set<std::string> mods;

    for (auto& loadedMod : m_LoadedMods)
    {
        mods.insert(loadedMod.first);
    }

    return mods;
}

std::unordered_map<std::string, ModManager::LoadedMod>& ModManager::GetLoadedMods()
{
    return m_LoadedMods;
}

void ModManager::FindAvailableMods()
{
    std::filesystem::path modsFolderPath = std::format("{}\\mods", std::filesystem::current_path().string());

    if (std::filesystem::exists(modsFolderPath))
    {
        m_AvailableMods.clear();

        for (const auto& entry : std::filesystem::directory_iterator(modsFolderPath))
        {
            if (entry.path().extension() != ".dll")
            {
                continue;
            }

            m_AvailableMods.insert(entry.path().filename().stem().string());
        }
    }
    else
    {
        Logger::Warn("mods directory not found!");
    }
}

void ModManager::LoadAllMods()
{
    FindAvailableMods();

    std::filesystem::path iniFilePath = std::format("{}\\mods.ini", std::filesystem::current_path().string());

    if (std::filesystem::exists(iniFilePath))
    {
        mINI::INIFile iniFile = mINI::INIFile(iniFilePath.string());
        mINI::INIStructure iniStructure;

        iniFile.read(iniStructure);

        for (auto& mod : iniStructure)
        {
            if (m_AvailableMods.contains(mod.first))
            {
                LoadMod(mod.first, false);
            }
        }
    }

    SDK::GetInstance().GetModSelector()->UpdateAvailableMods(m_AvailableMods, GetActiveMods());
}

void ModManager::UnloadAllMods()
{
    AcquireSRWLockShared(&m_SrwLock);

    std::vector<std::string> modNames;

    for (auto& pair : m_LoadedMods)
    {
        modNames.push_back(pair.first);
    }

    ReleaseSRWLockShared(&m_SrwLock);

    for (std::string& modName : modNames)
    {
        UnloadMod(modName);
    }
}

void ModManager::LoadMod(const std::string& p_Name, const bool p_LiveLoad)
{
    ScopedExclusiveGuard scopedSharedGuard = ScopedExclusiveGuard(&m_SrwLock);

    if (m_LoadedMods.contains(p_Name))
    {
        Logger::Info("{} mod is already loaded.", p_Name);

        return;
    }

    std::string modFilePath = std::format("{}\\mods\\{}.dll", std::filesystem::current_path().string(), p_Name);

    if (!std::filesystem::exists(modFilePath))
    {
        Logger::Error("Couldn't find {} mod!", p_Name);

        return;
    }

    const HMODULE module = LoadLibraryA(modFilePath.c_str());

    if (module)
    {
        Logger::Info("Successfully loaded {} mod.", p_Name);
    }
    else
    {
        Logger::Error("Failed to load {} mod. Error: {}", p_Name, GetLastError());

        return;
    }

    const auto GetModInterfaceAddress = GetProcAddress(module, "GetModInterface");

    if (!GetModInterfaceAddress)
    {
        Logger::Error("Couldn't find mod interface! Make sure that the GetPluginInterface function is exported!");
        FreeLibrary(module);

        return;
    }

    const auto GetModeInterface = reinterpret_cast<GetModInterface_t>(GetModInterfaceAddress);

    IModInterface* modInterface = GetModeInterface();

    if (!modInterface)
    {
        Logger::Error("GetModeInterface returned null!");
        FreeLibrary(module);

        return;
    }

    LoadedMod mod;
    mod.m_Module = module;
    mod.m_ModInterface = modInterface;

    m_LoadedMods[p_Name] = mod;

    SDK::GetInstance().OnModLoaded(p_Name, modInterface, p_LiveLoad);
}

void ModManager::UnloadMod(const std::string& p_Name)
{
    ScopedExclusiveGuard scopedSharedGuard = ScopedExclusiveGuard(&m_SrwLock);
    auto iterator = m_LoadedMods.find(p_Name);

    if (iterator == m_LoadedMods.end())
    {
        return;
    }

    delete iterator->second.m_ModInterface;
    FreeLibrary(iterator->second.m_Module);

    m_LoadedMods.erase(iterator);
}

void ModManager::SetEnabledMods(const std::set<std::string>& p_Mods)
{
    std::vector<std::string> modsToUnload;

    AcquireSRWLockShared(&m_SrwLock);

    for (auto& pair : m_LoadedMods)
    {
        if (!p_Mods.contains(pair.first))
        {
            modsToUnload.push_back(pair.first);
        }
    }

    ReleaseSRWLockShared(&m_SrwLock);

    for (auto& mod : modsToUnload)
    {
        UnloadMod(mod);
    }

    std::vector<std::string> modsToLoad;

    AcquireSRWLockShared(&m_SrwLock);

    for (auto& mod : p_Mods)
    {
        if (m_LoadedMods.contains(mod))
        {
            continue;
        }

        modsToLoad.push_back(mod);
    }

    ReleaseSRWLockShared(&m_SrwLock);

    for (auto& mod : modsToLoad)
    {
        LoadMod(mod, true);
    }

    std::filesystem::path iniFilePath = std::format("{}\\mods.ini", std::filesystem::current_path().string());
    mINI::INIFile iniFile = mINI::INIFile(iniFilePath.string());
    mINI::INIStructure iniStructure;

    if (std::filesystem::exists(iniFilePath))
    {
        mINI::INIStructure oldIni;

        iniFile.read(oldIni);
    }

    for (auto& mod : p_Mods)
    {
        mINI::INIMap<std::string> map;

        iniStructure.set(mod, map);
    }

    iniFile.generate(iniStructure);

    SDK::GetInstance().GetModSelector()->UpdateAvailableMods(m_AvailableMods, GetActiveMods());
}

void ModManager::LockRead()
{
    AcquireSRWLockShared(&m_SrwLock);
}

void ModManager::UnlockRead()
{
    ReleaseSRWLockShared(&m_SrwLock);
}
