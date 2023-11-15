#include <Glacier/Resource/ZResourceStub.h>
#include <Glacier/Resource/ZResourceManager.h>

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

void ZResourceStub::AddRef()
{
	_InterlockedExchangeAdd(&m_nHeaderRef, 1);
	_InterlockedExchangeAdd(&m_nRef, 1);
}

void ZResourceStub::Release()
{
	long referenceCount = _InterlockedDecrement(&m_nRef);

	_InterlockedExchangeAdd(&m_nHeaderRef, -1);

	if (referenceCount == 0)
	{
		ResourceManager->ReleaseStub(this);
	}
}

ZResourcePtr ZResourceStub::GetInstallDependency(unsigned int nIndex) const
{
	return m_resourceReferences[nIndex].m_pResource;
}

IResourceInstaller* ZResourceStub::GetResourceInstaller() const
{
	return ResourceManager->GetResourceInstaller(m_nResourceTag);
}
