#include "DebugConsole.h"

#include <iostream>
#include <Windows.h>
#include <io.h>

#include "spdlog/common.h"

#include "Glacier/TArray.h"
#include "Glacier/ZString.h"

#include "Events.h"

#if _DEBUG
DebugConsole::DebugConsole() : m_Running(true), m_Redirected(false)
{
    AllocConsole();
    AttachConsole(GetCurrentProcessId());
    SetConsoleTitleA("Hitman Absolution SDK - Debug Console");

    StartRedirecting();

    m_InputThread = std::thread(
        [&]
        {
            std::string line;

            while (m_Running)
            {
                std::getline(std::cin, line);

                if (line.size() == 0)
                {
                    continue;
                }

                TArray<ZString> args{};
                std::vector<std::string> argumentStrings = util::Split(line, " ");

                for (const std::string& arg : argumentStrings)
                {
                    args.PushBack(arg);
                }

                Events::OnConsoleCommand->Call(args);
            }
        }
    );
}

DebugConsole::~DebugConsole()
{
    m_Running = false;

    if (m_InputThread.joinable())
    {
        // Send a key event to the console so getline unblocks.
        INPUT_RECORD inputs[2]{};

        inputs[0].EventType = KEY_EVENT;
        inputs[0].Event.KeyEvent.bKeyDown = TRUE;
        inputs[0].Event.KeyEvent.uChar.AsciiChar = VK_RETURN;
        inputs[0].Event.KeyEvent.wRepeatCount = 1;
        inputs[0].Event.KeyEvent.wVirtualKeyCode = VK_RETURN;
        inputs[0].Event.KeyEvent.wVirtualScanCode = MapVirtualKeyA(VK_RETURN, MAPVK_VK_TO_VSC);

        inputs[1] = inputs[0];
        inputs[1].Event.KeyEvent.bKeyDown = FALSE;

        DWORD eventsWritten;
        WriteConsoleInputA(GetStdHandle(STD_INPUT_HANDLE), inputs, 2, &eventsWritten);

        m_InputThread.join();
    }

    StopRedirecting();
    FreeConsole();
}

void DebugConsole::StartRedirecting()
{
    if (m_Redirected)
    {
        StopRedirecting();
    }

    m_Redirected = true;

    m_OriginalStdin = _dup(0);
    m_OriginalStdout = _dup(1);
    m_OriginalStderr = _dup(2);

    FILE* con;
    freopen_s(&con, "CONIN$", "r", stdin);
    freopen_s(&con, "CONOUT$", "w", stderr);
    freopen_s(&con, "CONOUT$", "w", stdout);

    SetConsoleOutputCP(CP_UTF8);
}

void DebugConsole::StopRedirecting()
{
    if (!m_Redirected)
    {
        return;
    }

    m_Redirected = false;

    _dup2(m_OriginalStdin, 0);
    _dup2(m_OriginalStdout, 1);
    _dup2(m_OriginalStderr, 2);
}
#endif
