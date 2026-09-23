#include "ModSettings.h"

#include <filesystem>
#include <utility>
#include <ini.h>

ModSettings::ModSettings(std::string p_ModName, std::filesystem::path p_ModDirectory)
    : m_ModName(std::move(p_ModName)), m_ModDirectory(std::move(p_ModDirectory))
{
    Reload();
}

ModSettings::~ModSettings()
{
    Save();
}

void ModSettings::Reload()
{
    std::unique_lock<std::shared_mutex> lock(m_SettingsMutex);

    m_Settings.clear();

    std::filesystem::path settingsIniPath = m_ModDirectory / (m_ModName + ".ini");

    if (!std::filesystem::exists(settingsIniPath))
    {
        return;
    }

    mINI::INIFile file(settingsIniPath.string());

    mINI::INIStructure ini;

    file.read(ini);

    for (auto const& it : ini)
    {
        auto const& sectionName = it.first;
        auto const& sectionItems = it.second;

        std::unordered_map<std::string, std::string> section;

        for (auto const& it2 : sectionItems)
        {
            section[it2.first] = it2.second;
        }

        m_Settings[sectionName] = section;
    }
}

void ModSettings::Save()
{
    std::unique_lock<std::shared_mutex> lock(m_SettingsMutex);

    std::filesystem::path settingsIniPath = m_ModDirectory / (m_ModName + ".ini");

    mINI::INIFile file(settingsIniPath.string());

    mINI::INIStructure ini;

    for (auto const& it : m_Settings)
    {
        auto const& sectionName = it.first;
        auto const& sectionItems = it.second;

        mINI::INIMap<std::string> section;

        for (auto const& it2 : sectionItems)
        {
            section.set(it2.first, it2.second);
        }

        ini.set(sectionName, section);
    }

    file.generate(ini, true);
}

bool ModSettings::HasSetting(const std::string& p_Section, const std::string& p_Name)
{
    std::shared_lock<std::shared_mutex> lock(m_SettingsMutex);
    return m_Settings.find(p_Section) != m_Settings.end() && m_Settings[p_Section].find(p_Name) != m_Settings[p_Section].end();
}

std::string ModSettings::GetSetting(const std::string& p_Section, const std::string& p_Name, const std::string& p_DefaultValue)
{
    std::shared_lock<std::shared_mutex> lock(m_SettingsMutex);

    auto sectionIt = m_Settings.find(p_Section);

    if (sectionIt == m_Settings.end())
    {
        return p_DefaultValue;
    }

    auto settingIt = sectionIt->second.find(p_Name);

    if (settingIt == sectionIt->second.end())
    {
        return p_DefaultValue;
    }

    return settingIt->second;
}

void ModSettings::SetSetting(const std::string& p_Section, const std::string& p_Name, const std::string& p_Value)
{
    {
        std::unique_lock<std::shared_mutex> lock(m_SettingsMutex);

        if (m_Settings.find(p_Section) == m_Settings.end())
        {
            m_Settings[p_Section] = std::unordered_map<std::string, std::string>();
        }

        m_Settings[p_Section][p_Name] = p_Value;
    }

    Save();
}

void ModSettings::RemoveSetting(const std::string& p_Section, const std::string& p_Name)
{
    {
        std::unique_lock<std::shared_mutex> lock(m_SettingsMutex);

        if (m_Settings.find(p_Section) == m_Settings.end())
        {
            return;
        }

        if (m_Settings[p_Section].find(p_Name) == m_Settings[p_Section].end())
        {
            return;
        }

        m_Settings[p_Section].erase(p_Name);
    }

    Save();
}
