#pragma once

class Settings
{
public:
    Settings();
    void Draw(const bool hasFocus);
    void Show();
    const bool ReadHMAIni() const;
    const bool PatchResources() const;
    const bool PauseOnFocusLoss() const;
    const bool MinimizeOnFocusLoss() const;
    void SetPauseOnFocusLoss(const bool pauseOnFocusLoss);
    void SetMinimizeOnFocusLoss(const bool minimizeOnFocusLoss);
    const bool IniFileHasKey(const std::string& section, const std::string& key);
    void GetValueFromIniFile(const std::string& section, const std::string& key, bool& outValue);
    void UpdateIniFile(const std::string& key, const bool value);

private:
    bool isOpen;
    bool readHMAIni;
    bool patchResources;
    bool pauseOnFocusLoss;
    bool minimizeOnFocusLoss;
};
