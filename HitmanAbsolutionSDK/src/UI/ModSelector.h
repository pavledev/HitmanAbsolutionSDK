#pragma once

#include <string>
#include <vector>
#include <set>

#include "Common.h"

namespace UI
{
    class ModSelector
    {
      private:
        struct Mod
        {
            std::string m_Name;
            bool m_IsEnabled;
        };

      public:
        ModSelector();

        void Draw(bool hasFocus);
        void Show();
        void EnableSelectedMods();
        void UpdateAvailableMods(const std::set<std::string>& p_Mods, const std::set<std::string>& p_ActiveMods);

      private:
        bool m_ShowWindow;

        SRWLOCK srwLock{};
        std::vector<Mod> availableMods;
    };
}
