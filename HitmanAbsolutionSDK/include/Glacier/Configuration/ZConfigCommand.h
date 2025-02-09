#pragma once

#include "../ZString.h"

class HitmanAbsolutionSDK_API ZConfigCommand
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

	static ZConfigCommand* First();
	ZConfigCommand* Next() const;
	static void ExecuteCommand(const ZString& pCommandName, const char* argv);
	ZString Name() const;

protected:
    const char* m_pszName;
    ZConfigCommand* m_pNext;
};
