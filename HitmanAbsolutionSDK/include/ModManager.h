#pragma once

#include <Windows.h>
#include <set>
#include <string>
#include <unordered_map>
#include <memory>

class IModInterface;

class ModManager
{
  public:
    struct LoadedMod
    {
        IModInterface* m_ModInterface;
        HMODULE m_Module;
    };

    ModManager();
    ~ModManager();
    std::set<std::string> GetActiveMods();
    std::unordered_map<std::string, LoadedMod>& GetLoadedMods();
    void FindAvailableMods();
    void LoadAllMods();
    void UnloadAllMods();
    void LoadMod(const std::string& p_Name, const bool p_LiveLoad);
    void UnloadMod(const std::string& p_Name);
    void SetEnabledMods(const std::set<std::string>& p_Mods);
    void LockRead();
    void UnlockRead();

  private:
    std::set<std::string> m_AvailableMods;
    std::unordered_map<std::string, LoadedMod> m_LoadedMods;
    SRWLOCK m_SrwLock;
};
