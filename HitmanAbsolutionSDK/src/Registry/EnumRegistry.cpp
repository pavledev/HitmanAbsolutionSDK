#include <fstream>

#include "rapidjson/istreamwrapper.h"
#include "rapidjson/document.h"

#include "Registry/EnumRegistry.h"
#include "Logging.h"

EnumRegistry& EnumRegistry::GetInstance()
{
    static EnumRegistry instance;

    return instance;
}

void EnumRegistry::Load()
{
    std::ifstream inputFileStream = std::ifstream("assets/Enums.json");
    rapidjson::IStreamWrapper streamWrapper(inputFileStream);
    rapidjson::Document document;

    document.ParseStream(streamWrapper);

    const rapidjson::Value& enums2 = document["enums"];

    for (rapidjson::Value::ConstValueIterator it = enums2.Begin(); it != enums2.End(); ++it)
    {
        const rapidjson::Value& object = it->GetObj();
        const rapidjson::Value& items = object["items"];
        std::map<int32_t, std::string> items2;

        for (rapidjson::Value::ConstValueIterator it2 = items.Begin(); it2 != items.End(); ++it2)
        {
            const rapidjson::Value& object2 = it2->GetObj();

            items2.insert(std::make_pair(object2["value"].GetInt(), object2["name"].GetString()));
        }

        m_Enums.insert(std::make_pair(object["name"].GetString(), items2));
    }

    Logger::Info("Sucessfully loaded enums.");
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
