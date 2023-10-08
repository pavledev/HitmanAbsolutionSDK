#include <Glacier/Resource/ZResourcePtr.h>
#include <Glacier/Resource/ZResourceStub.h>

ZResourceStub* ZResourcePtr::GetResourceStub() const
{
    return m_pResourceStub;
}

void* ZResourcePtr::GetRawPointer() const
{
    if (m_pResourceStub && m_pResourceStub->GetResourceStatus() == RESOURCE_STATUS_VALID)
    {
        return m_pResourceStub->GetResourceData();
    }

    return nullptr;
}
