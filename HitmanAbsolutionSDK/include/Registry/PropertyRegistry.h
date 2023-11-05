#pragma once

#include <string>
#include <unordered_map>

#include "Common.h"

class HitmanAbsolutionSDK_API PropertyRegistry
{
public:
	static PropertyRegistry& GetInstance();
	void Load();
	const std::string& GetPropertyName(const unsigned int propertyID) const;

private:
	PropertyRegistry() = default;
	PropertyRegistry(const PropertyRegistry& other) = delete;
	PropertyRegistry& operator=(const PropertyRegistry& other) = delete;

	std::unordered_map<unsigned int, std::string> properties;
};
