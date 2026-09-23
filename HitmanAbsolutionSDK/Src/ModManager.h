#pragma once

#include <filesystem>
#include <shared_mutex>
#include <unordered_map>
#include <unordered_set>
#include <Windows.h>

#include "ModSettings.h"

class IModInterface;

class ModManager
{
  public:
    struct LoadedMod
    {
        IModInterface* m_ModInterface;
        HMODULE m_Module;
        ModSettings* m_Settings;
    };

    ModManager();
    ~ModManager();

    void LoadAllMods();
    void FindAvailableMods();
    std::unordered_set<std::string> GetAvailableMods();
    std::unordered_set<std::string> GetActiveMods();
    void SetActiveMods(const std::unordered_set<std::string>& p_Mods);
    void LoadMod(const std::string& p_Name, bool p_LiveLoad);
    void UnloadMod(const std::string& p_Name);
    void ReloadMod(const std::string& p_Name);
    void UnloadAllMods();
    void ReloadAllMods();

    IModInterface* GetModByName(const std::string& p_Name);
    ModSettings* GetModSettings(IModInterface* p_ModInterface);

    std::vector<IModInterface*> GetLoadedMods() const
    {
        return m_ModList;
    }

    void LockRead()
    {
        m_Mutex.lock_shared();
    }

    void UnlockRead()
    {
        m_Mutex.unlock_shared();
    }

  private:
    std::unordered_set<std::string> m_IncompatibleMods;
    std::unordered_set<std::string> m_AvailableMods;
    std::unordered_set<std::string> m_AvailableModsLower;
    std::vector<IModInterface*> m_ModList;
    std::unordered_map<std::string, LoadedMod> m_LoadedMods;
    std::shared_mutex m_Mutex;
};
