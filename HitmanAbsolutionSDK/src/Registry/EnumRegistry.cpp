#include <fstream>

#include "rapidjson/istreamwrapper.h"
#include "rapidjson/document.h"

#include "Registry/EnumRegistry.h"
#include "Logging.h"
#include "SDK.h"
#include "Resources.h"

EnumRegistry& EnumRegistry::GetInstance()
{
    static EnumRegistry instance;

    return instance;
}

void EnumRegistry::Load()
{
    const std::string_view enumsJson = SDK::GetInstance().GetTextResource(IDR_ENUMS);

    if (enumsJson.empty())
    {
        Logger::Error("Failed to load enums resource.");
        return;
    }

    rapidjson::Document document;
    document.Parse(enumsJson.data(), enumsJson.size());

    if (document.HasParseError())
    {
        Logger::Error("Failed to parse enums resource.");
        return;
    }

    const rapidjson::Value& enums = document["enums"];

    for (auto it = enums.Begin(); it != enums.End(); ++it)
    {
        const rapidjson::Value& object = it->GetObj();
        const rapidjson::Value& items = object["items"];
        std::map<int32_t, std::string> enumItems;

        for (auto it2 = items.Begin(); it2 != items.End(); ++it2)
        {
            const rapidjson::Value& item = it2->GetObj();

            enumItems.emplace(item["value"].GetInt(), item["name"].GetString());
        }

        m_Enums.emplace(object["name"].GetString(), std::move(enumItems));
    }

    Logger::Info("Successfully loaded enums.");
}

const std::map<int32_t, std::string>& EnumRegistry::GetEnum(const std::string& p_TypeName)
{
    auto it = m_Enums.find(p_TypeName);

    if (it != m_Enums.end())
    {
        return it->second;
    }

    static std::map<int32_t, std::string> enumItems;

    return enumItems;
}
