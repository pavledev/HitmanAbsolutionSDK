#include <Glacier/Entity/ZEntityRef.h>
#include <Glacier/Entity/ZObjectRef.h>

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
