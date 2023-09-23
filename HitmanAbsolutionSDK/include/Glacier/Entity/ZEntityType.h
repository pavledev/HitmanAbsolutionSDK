#pragma once

#include "../Templates/TArray.h"

struct SPropertyData;
struct SInterfaceData;
struct SExposedEntityData;
struct SSubsetData;
struct SPinData;
template <typename A, class B, class C, class D, class E> class ZEvent;
class ZEventNull;
class ZEntityRef;

class ZEntityType
{
private:
	enum EntityTypeOwner
	{
		EOWNED_BY_FACTORY = 0,
		EOWNED_BY_ENTITY = 1
	};

	int m_nBorrowedPointersMask; //Offset = 0x0 Size = 0x4
	TArray<SPropertyData>* m_pPropertyData; //Offset = 0x4 Size = 0x4
	TArray<SInterfaceData>* m_pInterfaceData; //Offset = 0x8 Size = 0x4
	TArray<SExposedEntityData>* m_pExposedEntityData; //Offset = 0xC Size = 0x4
	TArray<SSubsetData>* m_pSubsets; //Offset = 0x10 Size = 0x4
	TArray<SPinData>* m_pInputPins; //Offset = 0x14 Size = 0x4
	TArray<SPinData>* m_pOutputPins; //Offset = 0x18 Size = 0x4
	ZEvent<ZEntityRef const&, ZEventNull, ZEventNull, ZEventNull, ZEventNull>* m_pDeletionListeners; //Offset = 0x1C Size = 0x4
};
