#include <Glacier/TypeInfo/IType.h>
#include <Glacier/Entity/ZVariantRef.h>
#include <Glacier/Memory/ZMemoryManager.h>
#include <Glacier/Memory/IAllocator.h>

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
