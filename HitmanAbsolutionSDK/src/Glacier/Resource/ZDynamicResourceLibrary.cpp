#include <Glacier/Resource/ZDynamicResourceLibrary.h>
#include <Glacier/Resource/ZResourceManager.h>
#include <Glacier/ZThread.h>

#include <Global.h>
#include <Function.h>

ZDynamicResourceLibrary::ZDynamicResourceLibrary(const ZRuntimeResourceID& ridHeaderLibrary, bool bDelayedCreateEntities, unsigned int nNumInstances) : m_iEntitiesNUM(0)
{
	Function::CallMethod<ZDynamicResourceLibrary*, const ZRuntimeResourceID&, bool, unsigned int>(BaseAddress + 0x555C70, this, ridHeaderLibrary, bDelayedCreateEntities, nNumInstances);
}

ZDynamicResourceLibrary::~ZDynamicResourceLibrary()
{
	if (m_bLoadingHeader)
	{
		const ZMemberDelegate<ZDynamicResourceLibrary, void(const ZRuntimeResourceID&)> delegate(this, &ZDynamicResourceLibrary::OnHeaderLibraryReady);

		m_HeaderLibrary.RemoveStatusChangedListener(delegate);

		m_bLoadingHeader = false;
	}

	if (m_HeaderLibrary.GetResourceStub())
	{
		if (m_pCurrentLibrarySet)
		{
			if (!m_bReady)
			{
				const ZMemberDelegate<ZDynamicResourceLibrary, void()> delegate(this, &ZDynamicResourceLibrary::OnResourceLibrariesReady);

				m_pCurrentLibrarySet->RemoveReadyCallback(delegate);
			}

			m_pCurrentLibrarySet->Release();
		}

		m_pCurrentLibrarySet = nullptr;

		DeleteAllEntities();
	}
}

ZRuntimeResourceID ZDynamicResourceLibrary::GetLibraryResourceID()
{
	return m_ridHeaderLibrary;
}

ZEntityRef ZDynamicResourceLibrary::GetEntity(int nIndex)
{
	if (m_bDelayedCreateEntities && m_bReady && m_iEntitiesNUM != m_Entities.Size())
	{
		CreateEntities();
	}

	if (m_Entities.Size() > 0 && nIndex < m_Entities.Size())
	{
		return m_Entities[nIndex];
	}

	return ZEntityRef{};
}

void ZDynamicResourceLibrary::WaitForResources()
{
	while (!IsReady())
	{
		ResourceManager->Update(true);
		ZThread::Sleep(20);
	}
}

bool ZDynamicResourceLibrary::IsReady()
{
	if (m_bFailed)
	{
		return true;
	}

	return m_bReady;
}

bool ZDynamicResourceLibrary::IsFailed()
{
	return m_bFailed;
}

unsigned int ZDynamicResourceLibrary::GetEntityCount() const
{
	return m_iEntitiesNUM;
}

void ZDynamicResourceLibrary::CreateEntities()
{
	Function::CallMethod<ZDynamicResourceLibrary*>(BaseAddress + 0x30A450, this);
}

void ZDynamicResourceLibrary::DeleteAllEntities()
{
	Function::CallMethod<ZDynamicResourceLibrary*>(BaseAddress + 0x612F80, this);
}

void ZDynamicResourceLibrary::OnHeaderLibraryReady(const ZRuntimeResourceID& rid)
{
	Function::CallMethod<ZDynamicResourceLibrary*, const ZRuntimeResourceID&>(BaseAddress + 0x34010, this, rid);
}

void ZDynamicResourceLibrary::OnResourceLibrariesReady()
{
	Function::CallMethod<ZDynamicResourceLibrary*>(BaseAddress + 0x103500, this);
}
