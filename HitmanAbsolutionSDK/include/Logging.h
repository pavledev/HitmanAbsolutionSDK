#pragma once

#include <utility>

#include "spdlog/spdlog.h"

#include "IModSDK.h"

namespace Logger
{
    namespace detail
    {
        inline void Dispatch(spdlog::level::level_enum p_Level, std::string_view p_Msg)
        {
            SDK().Log(p_Level, p_Msg);
        }
    }

    template<typename... Args> void Error(spdlog::format_string_t<Args...> p_Format, Args&&... p_Args)
    {
        detail::Dispatch(spdlog::level::err, fmt::format(p_Format, std::forward<Args>(p_Args)...));
    }

    template<typename... Args> void Warn(spdlog::format_string_t<Args...> p_Format, Args&&... p_Args)
    {
        detail::Dispatch(spdlog::level::warn, fmt::format(p_Format, std::forward<Args>(p_Args)...));
    }

    template<typename... Args> void Info(spdlog::format_string_t<Args...> p_Format, Args&&... p_Args)
    {
        detail::Dispatch(spdlog::level::info, fmt::format(p_Format, std::forward<Args>(p_Args)...));
    }

    template<typename... Args> void Debug(spdlog::format_string_t<Args...> p_Format, Args&&... p_Args)
    {
        detail::Dispatch(spdlog::level::debug, fmt::format(p_Format, std::forward<Args>(p_Args)...));
    }

    template<typename... Args> void Trace(spdlog::format_string_t<Args...> p_Format, Args&&... p_Args)
    {
        detail::Dispatch(spdlog::level::trace, fmt::format(p_Format, std::forward<Args>(p_Args)...));
    }
}
