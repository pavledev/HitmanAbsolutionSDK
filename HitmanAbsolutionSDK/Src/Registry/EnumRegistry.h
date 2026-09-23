#pragma once

#include <string>
#include <unordered_map>
#include <map>

class EnumRegistry
{
  public:
    static EnumRegistry& GetInstance();
    void Load();
    const std::map<int32_t, std::string>& GetEnum(const std::string& p_TypeName) const;

  private:
    EnumRegistry() = default;
    EnumRegistry(const EnumRegistry& other) = delete;
    EnumRegistry& operator=(const EnumRegistry& other) = delete;

    std::unordered_map<std::string, std::map<int32_t, std::string>> m_Enums;
};
