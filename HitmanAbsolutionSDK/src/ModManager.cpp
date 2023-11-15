#include <filesystem>

#define MINI_CASE_SENSITIVE

#include <ini.h>

#include "ModManager.h"
#include "Logger.h"
#include "SDK.h"
#include "ModInterface.h"

ModManager::ModManager()
{
	InitializeSRWLock(&srwLock);
}

std::set<std::string> ModManager::GetActiveMods()
{
	ScopedSharedGuard scopedSharedGuard = ScopedSharedGuard(&srwLock);
	std::set<std::string> mods;

	for (auto& loadedMod : loadedMods)
	{
		mods.insert(loadedMod.first);
	}

	return mods;
}

std::unordered_map<std::string, ModManager::LoadedMod>& ModManager::GetLoadedMods()
{
	return loadedMods;
}

void ModManager::FindAvailableMods()
{
	std::filesystem::path modsFolderPath = std::format("{}\\mods", std::filesystem::current_path().string());

	if (std::filesystem::exists(modsFolderPath))
	{
		availableMods.clear();

		for (const auto& entry : std::filesystem::directory_iterator(modsFolderPath))
		{
			if (entry.path().extension() != ".dll")
			{
				continue;
			}

			availableMods.insert(entry.path().filename().stem().string());
		}
	}
	else
	{
		Logger::GetInstance().Log(Logger::Level::Warning, "mods directory not found!");
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
			if (availableMods.contains(mod.first))
			{
				LoadMod(mod.first, false);
			}
		}
	}

	SDK::GetInstance().GetModSelector()->UpdateAvailableMods(availableMods, GetActiveMods());
}

void ModManager::UnloadAllMods()
{
	AcquireSRWLockShared(&srwLock);

	std::vector<std::string> modNames;

	for (auto& pair : loadedMods)
	{
		modNames.push_back(pair.first);
	}

	ReleaseSRWLockShared(&srwLock);

	for (std::string& modName : modNames)
	{
		UnloadMod(modName);
	}
}

void ModManager::LoadMod(const std::string& name, const bool liveLoad)
{
	ScopedExclusiveGuard scopedSharedGuard = ScopedExclusiveGuard(&srwLock);

	if (loadedMods.contains(name))
	{
		Logger::GetInstance().Log(Logger::Level::Info, "{} mod is already loaded.", name);

		return;
	}

	std::string modFilePath = std::format("{}\\mods\\{}.dll", std::filesystem::current_path().string(), name);

	if (!std::filesystem::exists(modFilePath))
	{
		Logger::GetInstance().Log(Logger::Level::Error, "Couldn't find {} mod!", name);

		return;
	}

	const HMODULE module = LoadLibraryA(modFilePath.c_str());

	if (module)
	{
		Logger::GetInstance().Log(Logger::Level::Info, "Successfully loaded {} mod.", name);
	}
	else
	{
		Logger::GetInstance().Log(Logger::Level::Error, "Failed to load {} mod. Error: {}", name, Logger::GetLastError());

		return;
	}

	const auto GetModInterfaceAddress = GetProcAddress(module, "GetModInterface");

	if (!GetModInterfaceAddress)
	{
		Logger::GetInstance().Log(Logger::Level::Error, "Couldn't find mod interface! Make sure that the GetPluginInterface function is exported!");
		FreeLibrary(module);

		return;
	}

	const auto GetModeInterface = reinterpret_cast<GetModInterface_t>(GetModInterfaceAddress);

	ModInterface* modInterface = GetModeInterface();

	if (!modInterface)
	{
		Logger::GetInstance().Log(Logger::Level::Error, "GetModeInterface returned null!");
		FreeLibrary(module);

		return;
	}

	LoadedMod mod;
	mod.module = module;
	mod.modInterface = modInterface;

	loadedMods[name] = mod;

	SDK::GetInstance().OnModLoaded(name, modInterface, liveLoad);
}

void ModManager::UnloadMod(const std::string& name)
{
	ScopedExclusiveGuard scopedSharedGuard = ScopedExclusiveGuard(&srwLock);
	auto iterator = loadedMods.find(name);

	if (iterator == loadedMods.end())
	{
		return;
	}

	delete iterator->second.modInterface;
	FreeLibrary(iterator->second.module);

	loadedMods.erase(iterator);
}

void ModManager::SetEnabledMods(const std::set<std::string>& mods)
{
	std::vector<std::string> modsToUnload;

	AcquireSRWLockShared(&srwLock);

	for (auto& pair : loadedMods)
	{
		if (!mods.contains(pair.first))
		{
			modsToUnload.push_back(pair.first);
		}
	}

	ReleaseSRWLockShared(&srwLock);

	for (auto& mod : modsToUnload)
	{
		UnloadMod(mod);
	}

	std::vector<std::string> modsToLoad;

	AcquireSRWLockShared(&srwLock);

	for (auto& mod : mods)
	{
		if (loadedMods.contains(mod))
		{
			continue;
		}

		modsToLoad.push_back(mod);
	}

	ReleaseSRWLockShared(&srwLock);

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

	for (auto& mod : mods)
	{
		mINI::INIMap<std::string> map;

		iniStructure.set(mod, map);
	}

	iniFile.generate(iniStructure, true);

	SDK::GetInstance().GetModSelector()->UpdateAvailableMods(availableMods, GetActiveMods());
}

void ModManager::LockRead()
{
	AcquireSRWLockShared(&srwLock);
}

void ModManager::UnlockRead()
{
	ReleaseSRWLockShared(&srwLock);
}
