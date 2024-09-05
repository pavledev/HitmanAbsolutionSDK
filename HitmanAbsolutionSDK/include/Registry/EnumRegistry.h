#pragma once

#include <string>
#include <unordered_map>
#include <map>

#include "Common.h"

class HitmanAbsolutionSDK_API EnumRegistry
{
public:
	static EnumRegistry& GetInstance();
	void Load();
	const std::map<int, std::string>& GetEnum(const std::string& typeName);

private:
	EnumRegistry() = default;
	EnumRegistry(const EnumRegistry& other) = delete;
	EnumRegistry& operator=(const EnumRegistry& other) = delete;

	std::unordered_map<std::string, std::map<int, std::string>> enums;
};
