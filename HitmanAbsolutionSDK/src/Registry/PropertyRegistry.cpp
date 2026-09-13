#include <fstream>

#include "rapidjson/istreamwrapper.h"
#include "rapidjson/document.h"

#include "Registry/PropertyRegistry.h"
#include "Logging.h"

PropertyRegistry& PropertyRegistry::GetInstance()
{
    static PropertyRegistry instance;

    return instance;
}

void PropertyRegistry::Load()
{
    std::ifstream inputFileStream = std::ifstream("assets/Properties.json");
    rapidjson::IStreamWrapper streamWrapper(inputFileStream);
    rapidjson::Document document;

    document.ParseStream(streamWrapper);

    const rapidjson::Value& properties2 = document["properties"];

    for (rapidjson::Value::ConstValueIterator it = properties2.Begin(); it != properties2.End(); ++it)
    {
        const rapidjson::Value& object = it->GetObj();
        std::string name = object["name"].GetString();
        uint32_t hash = object["hash"].GetUint();

        m_Properties.insert(std::make_pair(hash, name));
    }

    Logger::Info("Sucessfully loaded properties.");
}

const std::string& PropertyRegistry::GetPropertyName(const uint32_t p_PropertyID) const
{
    auto iterator = m_Properties.find(p_PropertyID);

    if (iterator != m_Properties.end())
    {
        return iterator->second;
    }

    static std::string propertyName;

    return propertyName;
}
