#include <algorithm>

#include "Utility/StringUtility.h"

std::string StringUtility::ToUpperCase(const std::string& string)
{
    std::string string2 = string;

    std::transform(string2.begin(), string2.end(), string2.begin(), toupper);

    return string2;
}

std::string StringUtility::ToLowerCase(const std::string& string)
{
    std::string string2 = string;

    std::transform(string2.begin(), string2.end(), string2.begin(), tolower);

    return string2;
}

void StringUtility::Replace(std::string& string, const std::string& from, const std::string& to)
{
    const size_t startPos = string.find(from);

    if (startPos == std::string::npos)
    {
        return;
    }

    string.replace(startPos, from.length(), to);
}

void StringUtility::ReplaceAll(std::string& string, const std::string& from, const std::string& to)
{
    if (from.empty())
    {
        return;
    }

    size_t startPos = 0;

    while ((startPos = string.find(from, startPos)) != std::string::npos)
    {
        string.replace(startPos, from.length(), to);

        startPos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}
