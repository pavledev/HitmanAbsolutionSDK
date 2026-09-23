#include <fstream>

#include "rapidjson/document.h"

#include "PropertyRegistry.h"
#include "Logging.h"
#include "ModSDK.h"
#include "Resources.h"

PropertyRegistry& PropertyRegistry::GetInstance()
{
    static PropertyRegistry instance;

    return instance;
}

void PropertyRegistry::Load()
{
    const std::string_view propertiesJson = ModSDK::GetInstance().GetTextResource(IDR_PROPERTIES);

    if (propertiesJson.empty())
    {
        Logger::Error("Failed to load properties resource.");
        return;
    }

    rapidjson::Document document;
    document.Parse(propertiesJson.data(), propertiesJson.size());

    if (document.HasParseError())
    {
        Logger::Error("Failed to parse properties resource.");
        return;
    }

    for (const auto& property : document["properties"].GetArray())
    {
        m_Properties.emplace(property["hash"].GetUint(), property["name"].GetString());
    }

    Logger::Info("Successfully loaded properties.");
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
