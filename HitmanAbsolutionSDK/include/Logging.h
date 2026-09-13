#pragma once

#include "spdlog/spdlog.h"

#include "Common.h"

HitmanAbsolutionSDK_API std::shared_ptr<spdlog::logger> GetMainLogger();

namespace Logger
{
    template<typename... Args> inline void Error(spdlog::format_string_t<Args...> format, Args&&... args)
    {
        GetMainLogger()->error(format, std::forward<Args>(args)...);
    }

    template<typename... Args> inline void Warn(spdlog::format_string_t<Args...> format, Args&&... args)
    {
        GetMainLogger()->warn(format, std::forward<Args>(args)...);
    }

    template<typename... Args> inline void Info(spdlog::format_string_t<Args...> format, Args&&... args)
    {
        GetMainLogger()->info(format, std::forward<Args>(args)...);
    }

    template<typename... Args> inline void Debug(spdlog::format_string_t<Args...> format, Args&&... args)
    {
        GetMainLogger()->debug(format, std::forward<Args>(args)...);
    }

    template<typename... Args> inline void Trace(spdlog::format_string_t<Args...> format, Args&&... args)
    {
        GetMainLogger()->trace(format, std::forward<Args>(args)...);
    }

    inline void Flush()
    {
        GetMainLogger()->flush();
    }
}
