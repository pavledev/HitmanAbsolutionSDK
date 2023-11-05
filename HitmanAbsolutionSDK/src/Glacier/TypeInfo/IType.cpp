#include <Glacier/TypeInfo/IType.h>
#include <Glacier/Entity/ZVariantRef.h>
#include <Glacier/Memory/ZMemoryManager.h>
#include <Glacier/Memory/IAllocator.h>
#include <Glacier/TypeInfo/ETypeInfoFlags.h>

#include <Global.h>

unsigned int IType::GetTypeSize() const
{
	return m_nTypeSize;
}

unsigned int IType::GetTypeAlignment() const
{
	return m_nTypeAlignment;
}

const char* IType::GetTypeName() const
{
	return pszTypeName;
}

void IType::PlacementConstruct(void* pObject, const void* pSource) const
{
	m_pTypeFunctions->placementCopyConstruct(pObject, pSource);
}

void IType::PlacementConstruct(void* pObject) const
{
	m_pTypeFunctions->placementConstruct(pObject);
}

void IType::Destruct(void* pObject) const
{
	m_pTypeFunctions->destruct(pObject);
}

void IType::Assign(void* pDestination, const void* pSource) const
{
	m_pTypeFunctions->assign(pDestination, pSource);
}

void IType::Assign(void* pObject, ZVariantRef rhs) const
{
	m_pTypeFunctions->assign(pObject, rhs.GetData());
}

void* IType::Construct() const
{
	IAllocator* normalAllocator = MemoryManager->GetNormalAllocator();
	void* data = normalAllocator->AllocateAligned(m_nTypeSize, m_nTypeAlignment, 0);

	m_pTypeFunctions->placementConstruct(data);

	return data;
}

void IType::DestructAndFree(void* pObject) const
{
	m_pTypeFunctions->destruct(pObject);

	IAllocator* normalAllocator = MemoryManager->GetNormalAllocator();

	normalAllocator->Free(pObject);
}

const bool IType::IsTEntityRef() const
{
	return m_nTypeInfoFlags & ETYPEINFOFLAG_TENTITYREF;
}

const bool IType::IsTResourcePtr() const
{
	return m_nTypeInfoFlags & ETYPEINFOFLAG_TRESOURCEPTR;
}

const bool IType::IsClass() const
{
	return m_nTypeInfoFlags & ETYPEINFOFLAG_CLASSTYPE;
}

const bool IType::IsEnum() const
{
	return m_nTypeInfoFlags & ETYPEINFOFLAG_ENUMTYPE;
}

const bool IType::IsContainer() const
{
	return m_nTypeInfoFlags & ETYPEINFOFLAG_CONTAINERTYPE;
}

const bool IType::IsArray() const
{
	return m_nTypeInfoFlags & ETYPEINFOFLAG_ARRAYTYPE;
}

const bool IType::IsFixedArray() const
{
	return m_nTypeInfoFlags & ETYPEINFOFLAG_FIXEDARRAYTYPE;
}
