#include <sstream>
#include <iomanip>

#include "Logger.h"
#include "Hooks.h"
#include "Utility/StringUtility.h"
#include "Connection/PipeServer.h"

Logger::Logger()
{
    file = nullptr;

    InitializeSRWLock(&srwLock);

    if (AllocConsole())
    {
        freopen_s(&file, "CONOUT$", "w", stdout);
        freopen_s(&file, "CONOUT$", "w", stderr);
    }
}

Logger::~Logger()
{
    if (file)
    {
        fclose(file);
    }

    FreeConsole();
}

Logger& Logger::GetInstance()
{
    static Logger instance;

    return instance;
}

unsigned int Logger::LevelToNumber(const Level level)
{
    switch (level)
    {
    case Level::Info:
        return 0;
    case Level::Warning:
        return 1;
    case Level::Error:
        return 2;
    default:
        return -1;
    }
}

const char* Logger::LevelToString(const Level level)
{
    switch (level)
    {
    case Level::Info:
        return "INFO: ";
    case Level::Warning:
        return "WARNING: ";
    case Level::Error:
        return "ERROR: ";
    default:
        return "";
    }
}

std::vector<Logger::Message>& Logger::GetMessages()
{
    return messages;
}

void Logger::ClearMessage(const unsigned int index)
{
    messages.erase(messages.begin() + index);
}

void Logger::ClearAllMessages()
{
    messages.clear();
}

std::string Logger::GetCurrentTime()
{
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm localTime;
    std::stringstream stringstream;

    localtime_s(&localTime, &time);

    stringstream << std::put_time(&localTime, "%H:%M:%S");

    return stringstream.str();
}

std::string Logger::GetLastError()
{
    //Get the error message ID, if any.
    DWORD errorMessageID = ::GetLastError();

    if (errorMessageID == 0)
    {
        return std::string(); //No error message has been recorded
    }

    LPSTR messageBuffer = nullptr;

    //Ask Win32 to give us the string version of that message ID.
    //The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

    //Copy the error message into a std::string.
    std::string message(messageBuffer, size);

    //Free the Win32's string's buffer.
    LocalFree(messageBuffer);

    return message;
}
