#pragma once

#include "../ZString.h"
#include "STypeFunctions.h"

struct STypeID;
class ZVariantRef;

class IType
{
public:
	unsigned int GetTypeSize() const;
	unsigned int GetTypeAlignment() const;
	const char* GetTypeName() const;
	void PlacementConstruct(void* pObject, const void* pSource) const;
	void PlacementConstruct(void* pObject) const;
	void Destruct(void* pObject) const;
	void Assign(void* pDestination, const void* pSource) const;
	void Assign(void* pObject, ZVariantRef rhs) const;
	void* Construct() const;
	void DestructAndFree(void* pObject) const;

private:
	const STypeFunctions* m_pTypeFunctions;
	unsigned short m_nTypeSize;
	unsigned char m_nTypeAlignment;
	unsigned char m_nTypeInfoFlags;
	const char* pszTypeName;
	STypeID* typeID;
	bool (*fromString)(void* param1, const IType* type, const ZString& string);
	unsigned int (*toString)(const void* param1, const IType* type, char* param3, unsigned int param4, const ZString& string);
};
