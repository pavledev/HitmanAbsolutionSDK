#include <fstream>

#include "rapidjson/istreamwrapper.h"
#include "rapidjson/document.h"

#include "Registry/PropertyRegistry.h"
#include "Logger.h"

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
		unsigned int hash = object["hash"].GetUint();

		properties.insert(std::make_pair(hash, name));
	}

	Logger::GetInstance().Log(Logger::Level::Info, "Sucessfully loaded properties.");
}

const std::string& PropertyRegistry::GetPropertyName(const unsigned int propertyID) const
{
	auto iterator = properties.find(propertyID);

	if (iterator != properties.end())
	{
		return iterator->second;
	}

	static std::string propertyName;

	return propertyName;
}
