#pragma once

#include "Common.h"

class HitmanAbsolutionSDK_API GFxValue
{
public:
	enum ValueType
	{
		VT_Undefined = 0,
		VT_Null = 1,
		VT_Boolean = 2,
		VT_Number = 3,
		VT_String = 4,
		VT_StringW = 5,
		VT_Object = 6,
		VT_Array = 7,
		VT_DisplayObject = 8,
		VT_ConvertBoolean = 130,
		VT_ConvertNumber = 131,
		VT_ConvertString = 132,
		VT_ConvertStringW = 133
	};

	union ValueUnion
	{
		float NValue;
		bool BValue;
		const char* pString;
		const char** pStringManaged;
		const wchar_t* pStringW;
		void* pData;
	};

	class ObjectInterface;

	bool GetBool() const;
	void SetBool(const bool value);
	bool GetMember(const char* name, GFxValue* pval) const;
	bool SetMember(const char* name, const GFxValue& val);

private:
	ObjectInterface* pObjectInterface = nullptr;
	ValueType Type = VT_Undefined;
	ValueUnion Value;
};
