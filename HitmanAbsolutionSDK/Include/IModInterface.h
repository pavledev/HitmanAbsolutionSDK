#pragma once

#include <MinHook.h>

#define MINI_CASE_SENSITIVE

#include "ini.h"

#include "Glacier/ZInput.h"

#include "Logging.h"
#include "SDK.h"
#include "Utils/StringUtils.h"

class IModInterface
{
  public:
    virtual ~IModInterface() = default;

    virtual void Initialize() {}

    virtual void OnEngineInitialized() {}

    virtual void OnDrawUI(const bool hasFocus) {}

    virtual void OnDraw3D() {}

    virtual void OnDrawMenu() {}

    virtual void SetupUI()
    {
        SDK& sdk = SDK::GetInstance();
        ImGuiContext* imGuiContext = sdk.GetImGuiContext();

        if (!imGuiContext)
        {
            return;
        }

        ImGui::SetCurrentContext(imGuiContext);
        ImGui::SetAllocatorFunctions(sdk.GetImGuiMemAllocFunc(), sdk.GetImGuiMemFreeFunc(), sdk.GetImGuiUserDataAllocator());
    }

    void LoadConfiguration(const std::string& p_ModName);

  protected:
    void AddBindings()
    {
        std::string bindings = m_ModName + "Input={";

        for (auto bindingsIterator = m_IniStructure["Bindings"].begin(); bindingsIterator != m_IniStructure["Bindings"].end(); ++bindingsIterator)
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

        Globals::InputActionManager->AddBindings(bindings.c_str());
    }

  private:
    std::string GenerateBindingExpression(const std::string& actionName, const std::string& hotKeys)
    {
        const std::vector<std::string> hotKeys2 = util::Split(hotKeys, ",");

        struct Group
        {
            std::string tap;
            std::vector<std::string> mods;
        };

        std::vector<Group> groups;

        for (const std::string& hotKey : hotKeys2)
        {
            const std::vector<std::string> parts = util::Split(hotKey, "+");
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

    std::string m_ModName;
    mINI::INIStructure m_IniStructure;
};

using GetModInterface_t = IModInterface* (*)();

#define DECLARE_MOD(ModClass)                                          \
    extern "C" __declspec(dllexport) IModInterface* GetModInterface(); \
                                                                       \
    ModClass* GetModInstance()                                         \
    {                                                                  \
        return static_cast<ModClass*>(GetModInterface());              \
    }

#define DEFINE_MOD(ModClass)                                          \
    static IModInterface* ModClass##Singleton = nullptr;              \
                                                                      \
    extern "C" __declspec(dllexport) IModInterface* GetModInterface() \
    {                                                                 \
        if (!ModClass##Singleton)                                     \
            ModClass##Singleton = new ModClass();                     \
                                                                      \
        return ModClass##Singleton;                                   \
    }
