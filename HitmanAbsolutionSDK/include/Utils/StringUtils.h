#pragma once

#include <algorithm>
#include <string>
#include <vector>

#include "Common.h"

namespace util
{
    inline std::string ToUpperCase(const std::string& p_String)
    {
        std::string string = p_String;
        std::transform(string.begin(), string.end(), string.begin(), toupper);

        return string;
    }

    inline std::string ToLowerCase(const std::string& p_String)
    {
        std::string string = p_String;
        std::transform(string.begin(), string.end(), string.begin(), tolower);

        return string;
    }

    inline void Replace(std::string& p_String, const std::string& p_From, const std::string& p_To)
    {
        const size_t startPosition = p_String.find(p_From);

        if (startPosition == std::string::npos)
        {
            return;
        }

        p_String.replace(startPosition, p_From.length(), p_To);
    }

    inline void ReplaceAll(std::string& p_String, const std::string& p_From, const std::string& p_To)
    {
        if (p_From.empty())
        {
            return;
        }

        size_t startPosition = 0;

        while ((startPosition = p_String.find(p_From, startPosition)) != std::string::npos)
        {
            p_String.replace(startPosition, p_From.length(), p_To);
            startPosition += p_To.length();
        }
    }

    inline std::vector<std::string> Split(const std::string& p_String, const std::string& p_Delimeter)
    {
        std::vector<std::string> parts;

        size_t partStart = 0;
        size_t partEnd;

        while ((partEnd = p_String.find(p_Delimeter, partStart)) != std::string::npos)
        {
            parts.push_back(p_String.substr(partStart, partEnd - partStart));
            partStart = partEnd + p_Delimeter.length();
        }

        parts.push_back(p_String.substr(partStart));

        return parts;
    }

    inline bool Contains(const std::string& p_String, const std::string& p_Substring, bool p_CaseSensitive = true)
    {
        if (p_Substring.empty())
        {
            return true;
        }

        const auto result = std::ranges::search(
            p_String, p_Substring,
            [p_CaseSensitive](char p_Left, char p_Right)
            {
                if (p_CaseSensitive)
                {
                    return p_Left == p_Right;
                }

                return std::tolower(static_cast<uint8_t>(p_Left)) == std::tolower(static_cast<uint8_t>(p_Right));
            }
        );

        return !result.empty();
    }
};
