#include <Glacier/Entity/ZEntityRef.h>
#include <Glacier/Entity/ZObjectRef.h>
#include <Glacier/ZCurve.h>

ZEntityRef::ZEntityRef()
{
	m_pEntityTypePtrPtr = nullptr;
}

ZEntityRef::ZEntityRef(ZEntityType** entityTypePtrPtr)
{
	m_pEntityTypePtrPtr = entityTypePtrPtr;
}

bool ZEntityRef::operator==(const ZEntityRef& rhs) const
{
	return m_pEntityTypePtrPtr == rhs.m_pEntityTypePtrPtr;
}

bool ZEntityRef::operator!=(const ZEntityRef& rhs) const
{
	return m_pEntityTypePtrPtr != rhs.m_pEntityTypePtrPtr;
}

ZEntityImpl* ZEntityRef::GetEntityImpl() const
{
	if (m_pEntityTypePtrPtr)
	{
		return reinterpret_cast<ZEntityImpl*>(m_pEntityTypePtrPtr - 1);
	}

	return nullptr;
}

ZEntityType** ZEntityRef::GetEntityTypePtrPtr() const
{
	return m_pEntityTypePtrPtr;
}

void* ZEntityRef::QueryInterfacePtr(const STypeID* interfaceID) const
{
	SInterfaceData* interfaceData = (*m_pEntityTypePtrPtr)->GetInterfaceData(interfaceID);

	if (!interfaceData)
	{
		return nullptr;
	}

	return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(m_pEntityTypePtrPtr) + interfaceData->m_nInterfaceOffset);
}

TArray<SPropertyData>* ZEntityRef::GetProperties()
{
	return m_pEntityTypePtrPtr[0]->GetProperties();
}

ZVariant ZEntityRef::GetProperty(const ZString& propertyName)
{
	const unsigned int propertyID = Hash::Crc32(propertyName.ToCString(), propertyName.Length());

	return GetProperty(propertyID);
}

ZVariant ZEntityRef::GetProperty(const unsigned int propertyID)
{
	ZVariant variant{};
	ZEntityType* entityType = (*m_pEntityTypePtrPtr);
	SPropertyData* propertyData = entityType->GetPropertyData(propertyID);

	if (propertyData)
	{
		variant.Allocate(propertyData->m_pInfo->m_Type);

		const unsigned int propertyAddress = reinterpret_cast<uintptr_t>(m_pEntityTypePtrPtr) + propertyData->m_nPropertyOffset;

		if ((propertyData->m_pInfo->m_Flags & E_HAS_GETTER_SETTER) != 0)
		{
			propertyData->m_pInfo->m_PropetyGetter(reinterpret_cast<void*>(propertyAddress), variant.GetData(), propertyData->m_pInfo->m_nExtraData);
		}
		else
		{
			propertyData->m_pInfo->m_Type->pTypeInfo->PlacementConstruct(variant.GetData(), reinterpret_cast<void*>(propertyAddress));
		}
	}

	return variant;
}

bool ZEntityRef::SetProperty(const unsigned int propertyID, const ZVariantRef& value, bool invokeChangeHandlers)
{
	return SetPropertyValue(m_pEntityTypePtrPtr, propertyID, value, invokeChangeHandlers);
}

bool ZEntityRef::SetProperty(const ZString& propertyName, const ZVariantRef& value, bool invokeChangeHandlers)
{
	return SetProperty(Hash::Crc32(propertyName.ToCString(), propertyName.Length()), value, invokeChangeHandlers);
}

TArray<SPinData>* ZEntityRef::GetInputPins()
{
	return m_pEntityTypePtrPtr[0]->GetInputPins();
}

TArray<SPinData>* ZEntityRef::GetOutputPins()
{
	return m_pEntityTypePtrPtr[0]->GetOutputPins();
}
