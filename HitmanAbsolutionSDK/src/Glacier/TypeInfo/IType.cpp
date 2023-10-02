#include <Glacier/TypeInfo/IType.h>

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
