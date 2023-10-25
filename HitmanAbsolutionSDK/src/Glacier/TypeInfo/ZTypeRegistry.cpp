#include <Glacier/TypeInfo/ZTypeRegistry.h>

STypeID* ZTypeRegistry::GetType(const ZString& typeName)
{
	auto iterator = m_typeNameMap.Find(typeName);

	if (iterator != m_typeNameMap.End())
	{
		return iterator.Node().m_data.Value();
	}

	return nullptr;
}
