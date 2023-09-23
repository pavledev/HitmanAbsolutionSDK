#include "Utility/StringUtility.h"

std::string StringUtility::ToLowerCase(std::string string)
{
    for (size_t i = 0; i < string.length(); ++i)
    {
        string[i] = std::tolower(string[i]);
    }

    return string;
}
