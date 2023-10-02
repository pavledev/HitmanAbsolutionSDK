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
