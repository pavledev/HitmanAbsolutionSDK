#pragma once

#include <string>
#include <unordered_map>

class ResourceIDRegistry
{
public:
	static ResourceIDRegistry& GetInstance();
	void Load();
	std::string GetResourceID(const unsigned long long runtimeResourceID) const;
	const std::unordered_map<unsigned long long, std::string>& GetResourceIDs() const;
	const bool IsLoaded() const;

private:
	ResourceIDRegistry() = default;
	ResourceIDRegistry(const ResourceIDRegistry& other) = delete;
	ResourceIDRegistry& operator=(const ResourceIDRegistry& other) = delete;

	std::unordered_map<unsigned long long, std::string> resourceIDs;
	bool isLoaded = false;
};
