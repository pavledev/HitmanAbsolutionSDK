#pragma once

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

    virtual ECLASSTYPE GetType();
    virtual void Execute(const unsigned int param1, const char** param2) = 0;

protected:
    const char* m_pszName;
    ZConfigCommand* m_pNext;
};
