#include "Glacier/Input/ZInputActionManager.h"

#include "ModInterface.h"
#include "Global.h"

void ModInterface::LoadConfiguration(const std::string& modName)
{
    const std::filesystem::path iniFilePath = std::format("{}\\mods\\{}.ini", std::filesystem::current_path().string(), modName);

    if (!std::filesystem::exists(iniFilePath))
    {
        return;
    }

    const mINI::INIFile iniFile = mINI::INIFile(iniFilePath.string());

    iniFile.read(iniStructure);

    this->modName = modName;

    Logger::GetInstance().Log(Logger::Level::Info, "Successfully loaded .ini for {}", modName);
}

void ModInterface::AddBindings()
{
    std::string bindings = modName + "Input={";

    for (auto bindingsIterator = iniStructure["Bindings"].begin(); bindingsIterator != iniStructure["Bindings"].end(); ++bindingsIterator)
    {
        if (bindingsIterator->first == "EnableBindings")
        {
            if (bindingsIterator->second == "false")
            {
                return;
            }
            
            continue;
        }

        bindings += GenerateBindingExpression(bindingsIterator->first, bindingsIterator->second);
    }

    bindings += "};";

    InputActionManager->AddBindings(bindings.c_str());
}

std::string ModInterface::GenerateBindingExpression(const std::string& actionName, const std::string& hotKeys)
{
    const std::vector<std::string> hotKeys2 = StringUtility::Split(hotKeys, ',');

    struct Group
    {
        std::string tap;
        std::vector<std::string> mods;
    };

    std::vector<Group> groups;

    for (const std::string& hotKey : hotKeys2)
    {
        const std::vector<std::string> parts = StringUtility::Split(hotKey, '+');
        std::string tapKey;
        std::vector<std::string> modifiers;

        if (parts.size() == 1)
        {
            tapKey = parts[0];
        }
        else
        {
            tapKey = parts.back();

            modifiers.assign(parts.begin(), parts.end() - 1);
        }

        bool found = false;

        for (auto& grp : groups)
        {
            if (grp.tap == tapKey)
            {
                grp.mods.insert(grp.mods.end(), modifiers.begin(), modifiers.end());

                found = true;

                break;
            }
        }

        if (!found)
        {
            groups.push_back({ tapKey, modifiers });
        }
    }

    std::vector<std::string> groupExpressions;

    for (auto& grp : groups)
    {
        const std::string tap = grp.tap;
        const std::vector<std::string>& mods = grp.mods;
        std::string groupExpression;

        if (mods.empty())
        {
            groupExpression = "tap(kb," + tap + ")";
        }
        else
        {
            std::string modExpression;

            if (mods.size() == 1)
            {
                modExpression = "hold(kb," + mods[0] + ")";
            }
            else
            {
                modExpression = "|";

                for (const auto& mod : mods)
                {
                    modExpression += " hold(kb," + mod + ")";
                }
            }

            groupExpression = "& " + modExpression + " tap(kb," + tap + ")";
        }

        groupExpressions.push_back(groupExpression);
    }

    std::string bindingExpression = actionName + "=";

    if (groupExpressions.size() == 1)
    {
        bindingExpression += groupExpressions[0];
    }
    else
    {
        bindingExpression += "| " + groupExpressions[0] + " " + groupExpressions[1];

        for (size_t i = 2; i < groupExpressions.size(); i++)
        {
            bindingExpression = "| " + bindingExpression + " " + groupExpressions[i];
        }
    }

    bindingExpression += ";";

    return bindingExpression;
}
