#include <fstream>
#include <filesystem>
#include <format>

#include "Registry/ResourceIDRegistry.h"
#include "Logging.h"

ResourceIDRegistry& ResourceIDRegistry::GetInstance()
{
    static ResourceIDRegistry instance;

    return instance;
}

void ResourceIDRegistry::Load()
{
    std::ifstream ifstream = std::ifstream("assets/HashMap.txt");

    if (!ifstream.is_open())
    {
        Logger::Error("Failed to open HashMap.txt!");

        return;
    }

    ifstream.seekg(0, ifstream.end);

    size_t fileSize = static_cast<size_t>(ifstream.tellg());

    ifstream.seekg(0, ifstream.beg);

    std::vector<char> hashListData = std::vector<char>(fileSize, 0);
    unsigned int position = 0, lastPosition = 0;

    ifstream.read(hashListData.data(), fileSize);

    while (true)
    {
        if (hashListData.data()[position] == 0xA)
        {
            hashListData.data()[position] = 0;

            std::string line = std::string(&hashListData.data()[lastPosition]);

            uint64_t hash = std::stoull(line.substr(0, line.find(' ')), nullptr, 16);
            std::string resourceID = line.substr(line.find(' ') + 1);

            m_RuntimeResourceIDsToResourceIDs.insert(std::make_pair(hash, resourceID));
            m_ResourceIDsToRuntimeResourceIDs.insert(std::make_pair(resourceID, hash));

            lastPosition = position + 1;
        }

        position++;

        if (position > fileSize)
        {
            break;
        }
    }

    ifstream.close();

    m_IsLoaded = true;

    Logger::Info("Sucessfully loaded hash map.");
}

const char* ResourceIDRegistry::GetResourceID(const uint64_t p_RuntimeResourceID) const
{
    auto it = m_RuntimeResourceIDsToResourceIDs.find(p_RuntimeResourceID);

    if (it != m_RuntimeResourceIDsToResourceIDs.end())
    {
        return it->second.c_str();
    }

    return "";
}

uint64_t ResourceIDRegistry::GetRuntimeResourceID(const std::string& p_ResourceID) const
{
    auto it = m_ResourceIDsToRuntimeResourceIDs.find(p_ResourceID);

    if (it != m_ResourceIDsToRuntimeResourceIDs.end())
    {
        return it->second;
    }

    return -1;
}

const std::unordered_map<uint64_t, std::string>& ResourceIDRegistry::GetResourceIDs() const
{
    return m_RuntimeResourceIDsToResourceIDs;
}

bool ResourceIDRegistry::IsLoaded() const
{
    return m_IsLoaded;
}
