#pragma once

#include <Windows.h>
#include <unordered_set>
#include <vector>

#include <spdlog/spdlog.h>

#include <IImGuiRenderer.h>

namespace UI
{
    class ModSelector
    {
      private:
        struct ModEntry
        {
            std::string m_Name;
            bool m_IsEnabled;
        };

      public:
        ModSelector();

        void Draw(IImGuiRenderer* p_Renderer, bool p_HasFocus);

        void UpdateAvailableMods(
            const std::unordered_set<std::string>& p_Mods, const std::unordered_set<std::string>& p_IncompatibleMods,
            const std::unordered_set<std::string>& p_ActiveMods
        );

        void Show()
        {
            m_Open = true;
        }

      private:
        void ApplySelectedMods();

        bool m_Open = false;
        SRWLOCK m_Lock{};
        std::unordered_set<std::string> m_IncompatibleMods;
        std::vector<ModEntry> m_AvailableMods;
        bool m_ShouldShowWindow = false;
    };
}
