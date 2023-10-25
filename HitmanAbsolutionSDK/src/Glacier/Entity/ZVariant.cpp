#include <Glacier/Entity/ZVariant.h>
#include <Glacier/Entity/ZVariantRef.h>
#include <Glacier/TypeInfo/IType.h>
#include <Glacier/Memory/ZMemoryManager.h>
#include <Glacier/Memory/IAllocator.h>

ZVariant::ZVariant()
{
}

ZVariant::ZVariant(STypeID* type)
{
	Set(type);
}

ZVariant::ZVariant(STypeID* type, const void* pData) : ZObjectRef()
{
	Set(type, pData);
}

ZVariant::ZVariant(const ZVariantRef& rhs) : ZObjectRef()
{
	Set(rhs.GetTypeID(), rhs.GetData());
}

ZVariant::ZVariant(const ZVariant& rhs) : ZObjectRef()
{
	Set(rhs.m_TypeID, rhs.m_pData);
}

void ZVariant::Set(const ZVariantRef& rhs)
{
	Set(rhs.GetTypeID(), rhs.GetData());
}

void ZVariant::Set(const ZVariant& rhs)
{
	Set(rhs.m_TypeID, rhs.m_pData);
}

void ZVariant::Set(STypeID* type)
{
	IAllocator* normalAllocator = MemoryManager->GetNormalAllocator();

	m_TypeID = type;
	m_pData = normalAllocator->AllocateAligned(type->pTypeInfo->GetTypeSize(), type->pTypeInfo->GetTypeAlignment(), 0);

	m_TypeID->pTypeInfo->PlacementConstruct(m_pData);
}

void ZVariant::Set(STypeID* type, const void* pData)
{
	STypeID* variantTypeID = (*TypeRegistry)->GetType("ZVariant");
	STypeID* typeID = type;

	if (type == variantTypeID)
	{
		const ZVariant* variant = static_cast<const ZVariant*>(pData);

		typeID = variant->m_TypeID;
		pData = variant->m_pData;
	}

	if (typeID->flags == 1)
	{
		typeID = typeID->pSourceType;
	}

	if (m_TypeID == typeID)
	{
		m_TypeID->pTypeInfo->Assign(m_pData, pData);

		return;
	}

	Clear();

	m_TypeID = typeID;

	STypeID* voidTypeID = (*TypeRegistry)->GetType("void");

	if (typeID != voidTypeID)
	{
		IAllocator* normalAllocator = MemoryManager->GetNormalAllocator();

		if (typeID->flags == 2)
		{
			m_pData = normalAllocator->Allocate(4, 0);
		}
		else
		{
			m_pData = normalAllocator->Allocate(typeID->pTypeInfo->GetTypeSize(), 0);

			m_TypeID->pTypeInfo->PlacementConstruct(m_pData, pData);
		}
	}
}

void ZVariant::Allocate(STypeID* type)
{
	Clear();

	IAllocator* normalAllocator = MemoryManager->GetNormalAllocator();

	m_TypeID = type;
	m_pData = normalAllocator->AllocateAligned(type->pTypeInfo->GetTypeSize(), type->pTypeInfo->GetTypeAlignment(), 0);
}

void ZVariant::Clear()
{
	STypeID* voidTypeID = (*TypeRegistry)->GetType("void");

	if (m_TypeID != voidTypeID)
	{
		m_TypeID->pTypeInfo->DestructAndFree(m_pData);
	}

	m_TypeID = voidTypeID;
	m_pData = nullptr;
}
