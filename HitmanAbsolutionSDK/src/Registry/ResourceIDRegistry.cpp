#include <fstream>
#include <filesystem>
#include <format>

#include "Registry/ResourceIDRegistry.h"
#include "Logger.h"

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
        Logger::GetInstance().Log(Logger::Level::Error, "Failed to open HashMap.txt!");

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

            unsigned long long hash = std::stoull(line.substr(0, line.find(' ')), nullptr, 16);
            std::string resourceID = line.substr(line.find(' ') + 1);

            runtimeResourceIDsToResourceIDs.insert(std::make_pair(hash, resourceID));
            resourceIDsToRuntimeResourceIDs.insert(std::make_pair(resourceID, hash));

            lastPosition = position + 1;
        }

        position++;

        if (position > fileSize)
        {
            break;
        }
    }

    ifstream.close();

    isLoaded = true;

    Logger::GetInstance().Log(Logger::Level::Info, "Sucessfully loaded hash map.");
}

const char* ResourceIDRegistry::GetResourceID(const unsigned long long runtimeResourceID) const
{
    auto it = runtimeResourceIDsToResourceIDs.find(runtimeResourceID);

    if (it != runtimeResourceIDsToResourceIDs.end())
    {
        return it->second.c_str();
    }

    return "";
}

unsigned long long ResourceIDRegistry::GetRuntimeResourceID(const std::string& resourceID) const
{
    auto it = resourceIDsToRuntimeResourceIDs.find(resourceID);

    if (it != resourceIDsToRuntimeResourceIDs.end())
    {
        return it->second;
    }

    return -1;
}

const std::unordered_map<unsigned long long, std::string>& ResourceIDRegistry::GetResourceIDs() const
{
    return runtimeResourceIDsToResourceIDs;
}

const bool ResourceIDRegistry::IsLoaded() const
{
    return isLoaded;
}
