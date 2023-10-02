#pragma once

#include <Windows.h>
#include <set>
#include <string>
#include <unordered_map>
#include <memory>

class ModInterface;

class ModManager
{
public:
	struct LoadedMod
	{
		ModInterface* modInterface;
		HMODULE module;
	};

	ModManager();
	std::set<std::string> GetActiveMods();
	std::unordered_map<std::string, LoadedMod>& GetLoadedMods();
	void FindAvailableMods();
	void LoadAllMods();
	void UnloadAllMods();
	void LoadMod(const std::string& name, const bool liveLoad);
	void UnloadMod(const std::string& name);
	void SetEnabledMods(const std::set<std::string>& mods);
	void LockRead();
	void UnlockRead();

private:
	std::set<std::string> availableMods;
	std::unordered_map<std::string, LoadedMod> loadedMods;
	SRWLOCK srwLock;
};
