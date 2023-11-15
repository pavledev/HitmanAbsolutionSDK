#pragma once

#include <string>
#include <vector>
#include <set>

#include <Mutex.h>

class ModSelector
{
private:
    struct Mod
    {
        std::string name;
        bool isEnabled;
    };

public:
    ModSelector();
    void Draw(const bool hasFocus);
    void Show();
    void EnableSelectedMods();
    void UpdateAvailableMods(const std::set<std::string>& mods, const std::set<std::string>& activeMods);

private:
    bool isOpen;
    SRWLOCK srwLock{};
    std::vector<Mod> availableMods;
};
