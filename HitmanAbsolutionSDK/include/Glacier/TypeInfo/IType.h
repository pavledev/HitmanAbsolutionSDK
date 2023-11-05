#pragma once

#include "../ZString.h"
#include "STypeFunctions.h"

struct STypeID;
class ZVariantRef;

class HitmanAbsolutionSDK_API IType
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
    const bool IsTEntityRef() const;
    const bool IsTResourcePtr() const;
	const bool IsClass() const;
	const bool IsEnum() const;
	const bool IsContainer() const;
	const bool IsArray() const;
	const bool IsFixedArray() const;

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
