#include <Glacier/Resource/ZResourcePtr.h>
#include <Glacier/Resource/ZResourceStub.h>

#include <Global.h>
#include <Function.h>

ZResourcePtr::ZResourcePtr(const ZResourcePtr& rhs)
{
    m_pResourceStub = rhs.m_pResourceStub;

    if (m_pResourceStub)
    {
        m_pResourceStub->AddRef();
    }
}

ZResourcePtr::~ZResourcePtr()
{
    if (m_pResourceStub)
    {
        m_pResourceStub->Release();
        
        m_pResourceStub = nullptr;
    }
}

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

bool ZResourcePtr::IsReady() const
{
    if (!m_pResourceStub)
    {
        return false;
    }

    const EResourceStatus resourceStatus = m_pResourceStub->GetResourceStatus();

    if (resourceStatus == RESOURCE_STATUS_VALID)
    {
        return true;
    }

    return false;
}

bool ZResourcePtr::Exists() const
{
    return m_pResourceStub != 0;
}

bool ZResourcePtr::Failed() const
{
    return m_pResourceStub && m_pResourceStub->GetResourceStatus() == RESOURCE_STATUS_FAILED;
}

void ZResourcePtr::AddStatusChangedListener(const ZDelegate<void __cdecl(ZRuntimeResourceID const&)>& d)
{
    Function::CallMethod<ZResourcePtr*, const ZDelegate<void __cdecl(ZRuntimeResourceID const&)>&>(BaseAddress + 0x228300, this, d);
}

void ZResourcePtr::RemoveStatusChangedListener(const ZDelegate<void __cdecl(ZRuntimeResourceID const&)>& d)
{
    Function::CallMethod<ZResourcePtr*, const ZDelegate<void __cdecl(ZRuntimeResourceID const&)>&>(BaseAddress + 0x5DBBE0, this, d);
}

bool ZResourcePtr::operator==(const ZResourcePtr& rhs) const
{
    return m_pResourceStub == rhs.m_pResourceStub;
}
