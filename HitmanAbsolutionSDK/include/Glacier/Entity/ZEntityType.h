#pragma once

#include "../Templates/TArray.h"
#include "SPropertyData.h"
#include "SInterfaceData.h"
#include "SExposedEntityData.h"
#include "SSubsetData.h"
#include "SPinData.h"

#include <Common.h>

template <typename A, class B, class C, class D, class E> class ZEvent;
class ZEventNull;
class ZEntityRef;

class HitmanAbsolutionSDK_API ZEntityType
{
public:
	SInterfaceData* GetInterfaceData(const STypeID* type);
	SPropertyData* GetPropertyData(const unsigned int propertyID);
	TArray<SPropertyData>* GetProperties();
	TArray<SPinData>* GetInputPins();
	TArray<SPinData>* GetOutputPins();

private:
	enum EntityTypeOwner
	{
		EOWNED_BY_FACTORY = 0,
		EOWNED_BY_ENTITY = 1
	};

	int m_nBorrowedPointersMask;
	TArray<SPropertyData>* m_pPropertyData;
	TArray<SInterfaceData>* m_pInterfaceData;
	TArray<SExposedEntityData>* m_pExposedEntityData;
	TArray<SSubsetData>* m_pSubsets;
	TArray<SPinData>* m_pInputPins;
	TArray<SPinData>* m_pOutputPins;
	ZEvent<ZEntityRef const&, ZEventNull, ZEventNull, ZEventNull, ZEventNull>* m_pDeletionListeners;
};
