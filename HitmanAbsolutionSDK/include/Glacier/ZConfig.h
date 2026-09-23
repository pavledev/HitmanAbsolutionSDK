#pragma once

#include "ZString.h"

#include <Functions.h>

class ZConfigFloat;
class ZConfigInt;
class ZConfigString;

class ZConfigCommand
{
  public:
    enum ECLASSTYPE
    {
        ECLASS_FLOAT = 0,
        ECLASS_INT = 1,
        ECLASS_STRING = 2,
        ECLASS_UNKNOWN = 3
    };

    virtual ECLASSTYPE GetType() = 0;
    virtual void Execute(const unsigned int param1, const char** param2) = 0;

    static ZConfigCommand* First()
    {
        return Functions::ZConfigCommand_First->Call();
    }

    static void ExecuteCommand(const ZString& p_CommandName, const char* p_Argv)
    {
        Functions::ZConfigCommand_ExecuteCommand->Call(p_CommandName, p_Argv);
    }

    static ZConfigCommand* GetConfigVariable(const ZString& pszName)
    {
        return Functions::ZConfigCommand_GetConfigVariable->Call(pszName);
    }

    template<typename T> T* As()
    {
        return GetType() == GetClassType<T>() ? static_cast<T*>(this) : nullptr;
    }

  private:
    template<typename T> static ECLASSTYPE GetClassType()
    {
        if (std::is_same<T, ZConfigFloat>::value)
        {
            return ECLASSTYPE::ECLASS_FLOAT;
        }
        else if (std::is_same<T, ZConfigInt>::value)
        {
            return ECLASSTYPE::ECLASS_INT;
        }
        else if (std::is_same<T, ZConfigString>::value)
        {
            return ECLASSTYPE::ECLASS_STRING;
        }

        return ECLASSTYPE::ECLASS_UNKNOWN;
    }

  public:
    const char* m_pszName;
    ZConfigCommand* m_pNext;
};

class ZConfigFloatBase : public ZConfigCommand
{
  public:
    float m_Value;
};

class ZConfigFloat : public ZConfigFloatBase
{};

class ZConfigIntBase : public ZConfigCommand
{
  public:
    int32_t m_Value;
};

class ZConfigInt : public ZConfigIntBase
{};

class ZConfigStringBase : public ZConfigCommand
{
  public:
    char m_szValue[256];
};

class ZConfigString : public ZConfigStringBase
{};
