#include <filesystem>
#include <format>

#include <imgui.h>

#include <IconsMaterialDesign.h>

#define MINI_CASE_SENSITIVE

#include <ini.h>

#include <UI/Settings.h>
#include <SDK.h>

Settings::Settings()
{
    isOpen = false;
    readEngineIni = false;
    patchResources = false;

    GetValueFromIniFile("Settings", "ReadEngineIni", readEngineIni);
    GetValueFromIniFile("Settings", "PatchResources", patchResources);
}

void Settings::Draw(const bool hasFocus)
{
    if (!hasFocus || !isOpen)
    {
        return;
    }

    ImGui::PushFont(SDK::GetInstance().GetBoldFont());
    ImGui::SetNextWindowSize(ImVec2(600, 500), ImGuiCond_FirstUseEver);

    const bool isWindowVisible = ImGui::Begin(ICON_MD_SETTINGS " Settings", &isOpen, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);

    ImGui::PushFont(SDK::GetInstance().GetRegularFont());

    if (isWindowVisible)
    {
        if (ImGui::Checkbox("Read engine.ini", &readEngineIni))
        {
            UpdateIniFile("ReadEngineIni", readEngineIni);
        }

        if (ImGui::Checkbox("Patch Resources", &patchResources))
        {
            UpdateIniFile("PatchResources", patchResources);
        }
    }

    ImGui::PopFont();
    ImGui::End();
    ImGui::PopFont();
}

void Settings::Show()
{
    isOpen = true;
}

const bool Settings::ReadEngineIni() const
{
    return readEngineIni;
}

const bool Settings::PatchResources() const
{
    return patchResources;
}

void Settings::GetValueFromIniFile(const std::string& section, const std::string& key, bool& outValue)
{
    std::filesystem::path iniFilePath = std::format("{}\\HitmanAbsolutionSDK.ini", std::filesystem::current_path().string());

    if (!std::filesystem::exists(iniFilePath))
    {
        return;
    }

    mINI::INIFile iniFile = mINI::INIFile(iniFilePath.string());
    mINI::INIStructure iniStructure;

    iniFile.read(iniStructure);

    if (iniStructure.has(section) && iniStructure[section].has(key))
    {
        std::string value = iniStructure[section][key];

        std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c)
        {
            return std::tolower(c);
        });

        outValue = value == "true" || value == "1";
    }
}

void Settings::UpdateIniFile(const std::string& key, const bool value)
{
    std::filesystem::path iniFilePath = std::format("{}\\HitmanAbsolutionSDK.ini", std::filesystem::current_path().string());
    mINI::INIFile iniFile = mINI::INIFile(iniFilePath.string());
    mINI::INIStructure iniStructure;

    if (std::filesystem::exists(iniFilePath))
    {
        mINI::INIStructure oldIni;

        iniFile.read(oldIni);
    }

    iniStructure["Settings"][key] = value ? "true" : "false";

    iniFile.generate(iniStructure, true);
}
