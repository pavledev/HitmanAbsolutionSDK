#pragma once

class Settings
{
public:
    Settings();
    void Draw(const bool hasFocus);
    void Show();
    const bool ReadEngineIni() const;
    const bool PatchResources() const;
    void GetValueFromIniFile(const std::string& section, const std::string& key, bool& outValue);
    void UpdateIniFile(const std::string& key, const bool value);

private:
    bool isOpen;
    bool readEngineIni;
    bool patchResources;
};
