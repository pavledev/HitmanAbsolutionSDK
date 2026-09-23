#pragma once

#include <string>
#include <unordered_map>

class ResourceIDRegistry
{
  public:
    static ResourceIDRegistry& GetInstance();
    void Load();
    const char* GetResourceID(const uint64_t p_RuntimeResourceID) const;
    uint64_t GetRuntimeResourceID(const std::string& p_ResourceID) const;
    const std::unordered_map<uint64_t, std::string>& GetResourceIDs() const;
    bool IsLoaded() const;

  private:
    ResourceIDRegistry() = default;
    ResourceIDRegistry(const ResourceIDRegistry& other) = delete;
    ResourceIDRegistry& operator=(const ResourceIDRegistry& other) = delete;

    std::unordered_map<uint64_t, std::string> m_RuntimeResourceIDsToResourceIDs;
    std::unordered_map<std::string, uint64_t> m_ResourceIDsToRuntimeResourceIDs;
    bool m_IsLoaded = false;
};
