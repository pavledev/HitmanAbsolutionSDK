#include <Glacier/Resource/ZResourceStub.h>

EResourceStatus ZResourceStub::GetResourceStatus() const
{
	return static_cast<EResourceStatus>(m_eResourceStatus);
}

const ZRuntimeResourceID& ZResourceStub::GetRuntimeResourceID() const
{
	return m_ridResource;
}

void* ZResourceStub::GetResourceData() const
{
	return m_pResourceData;
}

unsigned int ZResourceStub::GetResourceTag() const
{
	return m_nResourceTag;
}
