#pragma once

#include "ZString.h"

#include <Functions.h>

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

    const char* m_pszName;
    ZConfigCommand* m_pNext;
};

class ZConfigFloatBase : public ZConfigCommand
{
  public:
    float m_Value;
};

class ZConfigFloat : public ZConfigFloatBase
{
  public:
    float GetVal() const
    {
        return m_Value;
    }
};

class ZConfigIntBase : public ZConfigCommand
{
  public:
    int32_t m_Value;
};

class ZConfigInt : public ZConfigIntBase
{
  public:
    int32_t GetVal() const
    {
        return m_Value;
    }
};
