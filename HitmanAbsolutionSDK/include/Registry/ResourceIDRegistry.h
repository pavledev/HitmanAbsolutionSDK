#pragma once

#include <string>
#include <unordered_map>

class ResourceIDRegistry
{
public:
	static ResourceIDRegistry& GetInstance();
	void Load();
	const char* GetResourceID(const unsigned long long runtimeResourceID) const;
	unsigned long long GetRuntimeResourceID(const std::string& resourceID) const;
	const std::unordered_map<unsigned long long, std::string>& GetResourceIDs() const;
	const bool IsLoaded() const;

private:
	ResourceIDRegistry() = default;
	ResourceIDRegistry(const ResourceIDRegistry& other) = delete;
	ResourceIDRegistry& operator=(const ResourceIDRegistry& other) = delete;

	std::unordered_map<unsigned long long, std::string> runtimeResourceIDsToResourceIDs;
	std::unordered_map<std::string, unsigned long long> resourceIDsToRuntimeResourceIDs;
	bool isLoaded = false;
};
