#pragma once

#include <string>

class StringUtility
{
public:
	static std::string ToUpperCase(const std::string& string);
	static std::string ToLowerCase(const std::string& string);
	static void Replace(std::string& string, const std::string& from, const std::string& to);
	static void ReplaceAll(std::string& string, const std::string& from, const std::string& to);
};
