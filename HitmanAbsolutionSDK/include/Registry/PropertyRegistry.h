#pragma once

#include <string>
#include <unordered_map>

#include "Common.h"

class PropertyRegistry
{
  public:
    static PropertyRegistry& GetInstance();
    void Load();
    const std::string& GetPropertyName(uint32_t p_PropertyID) const;

  private:
    PropertyRegistry() = default;
    PropertyRegistry(const PropertyRegistry& other) = delete;
    PropertyRegistry& operator=(const PropertyRegistry& other) = delete;

    std::unordered_map<uint32_t, std::string> m_Properties;
};
