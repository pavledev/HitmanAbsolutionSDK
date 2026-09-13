#include "Console.h"

#include <imgui.h>

#include "SDK.h"
#include "Events.h"

namespace UI
{
    Console::Console()
    {
        InitializeSRWLock(&m_Lock);
    }

    void Console::Draw(bool hasFocus)
    {
        if (!hasFocus)
        {
            return;
        }

        ImGui::PushFont(SDK::GetInstance().GetBoldFont());
        const auto isWindowExpanded =
            ImGui::Begin("CONSOLE", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
        ImGui::PushFont(SDK::GetInstance().GetRegularFont());

        ImGui::SetWindowCollapsed(true, ImGuiCond_Once);

        const auto& imGuiIO = ImGui::GetIO();
        ImGui::SetWindowSize(ImVec2(imGuiIO.DisplaySize.x - 60, 400), ImGuiCond_Always);
        ImGui::SetWindowPos(ImVec2(30, 80 * (imGuiIO.DisplaySize.y / 1800.f)), ImGuiCond_Always);

        if (isWindowExpanded)
        {
            const float footerHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();

            ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footerHeight), false);

            AcquireSRWLockShared(&m_Lock);

            ImGui::PushTextWrapPos();

            for (auto& logLine : m_LogLines)
            {
                ImVec4 color;
                bool isColored = false;

                switch (logLine.m_Level)
                {
                case spdlog::level::trace:
                    color = ImVec4(168.f / 255.f, 61.f / 255.f, 1.f, 1.f);
                    isColored = true;
                    break;

                case spdlog::level::debug:
                    color = ImVec4(61.f / 255.f, 129.f / 255.f, 1.f, 1.f);
                    isColored = true;
                    break;

                case spdlog::level::info:
                    break;

                case spdlog::level::warn:
                    color = ImVec4(1.f, 168.f / 255.f, 61.f / 255.f, 1.f);
                    isColored = true;
                    break;

                case spdlog::level::err:
                case spdlog::level::critical:
                    color = ImVec4(1.f, 69.f / 255.f, 69.f / 255.f, 1.f);
                    isColored = true;
                    break;

                case spdlog::level::off:
                    break;
                }

                if (isColored)
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, color);
                }

                ImGui::TextUnformatted(logLine.m_Text.c_str(), logLine.m_Text.c_str() + logLine.m_Text.size());

                if (isColored)
                {
                    ImGui::PopStyleColor();
                }
            }

            ImGui::PopTextWrapPos();

            // Auto scroll to bottom.
            if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            {
                ImGui::SetScrollHereY(1.0);
            }

            ReleaseSRWLockShared(&m_Lock);

            ImGui::EndChild();

            ImGui::Separator();

            if (!m_WasWindowExpanded)
            {
                ImGui::SetKeyboardFocusHere();
            }

            if (ImGui::InputText("##ConsoleCommand", m_Command, IM_ARRAYSIZE(m_Command), ImGuiInputTextFlags_EnterReturnsTrue))
            {
                Logger::Info("> {}", m_Command);

                TArray<ZString> args{};
                std::vector<std::string> argumentStrings = util::Split(m_Command, " ");

                for (const std::string& arg : argumentStrings)
                {
                    args.PushBack(arg);
                }

                Events::OnConsoleCommand->Call(args);

                m_Command[0] = '\0';

                ImGui::SetKeyboardFocusHere(-1);
            }

            ImGui::SetItemDefaultFocus();
        }

        m_WasWindowExpanded = isWindowExpanded;

        ImGui::PopFont();
        ImGui::End();
        ImGui::PopFont();
    }

    void Console::AddLogLine(spdlog::level::level_enum p_Level, const std::string& p_Text)
    {
        AcquireSRWLockExclusive(&m_Lock);

        m_LogLines.push_back(LogLine{ p_Level, std::string(p_Text.c_str(), p_Text.size()) });

        ReleaseSRWLockExclusive(&m_Lock);
    }
}
