#pragma once

#include <Windows.h>
#include <vector>

#include <spdlog/spdlog.h>

namespace UI
{
    class Console
    {
      private:
        struct LogLine
        {
            spdlog::level::level_enum m_Level;
            std::string m_Text;
        };

      public:
        Console();

        void Draw(bool hasFocus);
        void AddLogLine(spdlog::level::level_enum p_Level, const std::string& p_Text);

      private:
        std::vector<LogLine> m_LogLines{};
        SRWLOCK m_Lock{};

        bool m_WasWindowExpanded = false;
        char m_Command[2048] = {};
    };
}
