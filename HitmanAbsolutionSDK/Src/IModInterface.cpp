#include "IModInterface.h"

#include <filesystem>

void IModInterface::LoadConfiguration(const std::string& p_ModName)
{
    const std::filesystem::path iniFilePath = std::format("{}\\mods\\{}.ini", std::filesystem::current_path().string(), p_ModName);

    if (!std::filesystem::exists(iniFilePath))
    {
        return;
    }

    const mINI::INIFile iniFile = mINI::INIFile(iniFilePath.string());

    iniFile.read(m_IniStructure);

    this->m_ModName = p_ModName;

    Logger::Info("Successfully loaded .ini for {}", m_ModName);
}
