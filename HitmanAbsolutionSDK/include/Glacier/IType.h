#pragma once

struct STypeFunctions;
struct STypeID;
class ZString;

class IType //Size = 0x24
{
	const STypeFunctions* m_pTypeFunctions;
	unsigned short m_nTypeSize;
	unsigned char m_nTypeAlignment;
	unsigned char m_nTypeInfoFlags;
	const char* pszTypeName;
	STypeID* typeID;
	bool (*fromString)(void* param1, const IType* type, const ZString& string);
	unsigned int (*toString)(const void* param1, const IType* type, char* param3, unsigned int param4, const ZString& string);
};
