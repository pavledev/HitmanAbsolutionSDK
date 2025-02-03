#pragma once

#include <string>
#include <vector>

class StringUtility
{
public:
	static std::string ToUpperCase(const std::string& string);
	static std::string ToLowerCase(const std::string& string);
	static void Replace(std::string& string, const std::string& from, const std::string& to);
	static void ReplaceAll(std::string& string, const std::string& from, const std::string& to);
	static std::vector<std::string> Split(const std::string& string, const char delimiter);
	static std::vector<std::string> Split(const std::string& string, const std::string& delimiter);
};
