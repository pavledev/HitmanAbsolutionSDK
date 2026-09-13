#pragma once

#include "ZResource.h"
#include "ZEntity.h"

class ZDynamicResourceLibrary
{
  public:
    ZDynamicResourceLibrary(const ZRuntimeResourceID& p_HeaderLibrary, bool p_DelayedCreateEntities, uint32_t p_NumInstances) : m_iEntitiesNUM(0)
    {
        Functions::ZDynamicResourceLibrary_ZDynamicResourceLibrary->Call(this, p_HeaderLibrary, p_DelayedCreateEntities, p_NumInstances);
    }

    ~ZDynamicResourceLibrary()
    {
        if (m_bLoadingHeader)
        {
            const ZMemberDelegate<ZDynamicResourceLibrary, void(const ZRuntimeResourceID&)> delegate(
                this, &ZDynamicResourceLibrary::OnHeaderLibraryReady
            );

            m_HeaderLibrary.RemoveStatusChangedListener(delegate);

            m_bLoadingHeader = false;
        }

        if (m_HeaderLibrary.m_pResourceStub)
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

    ZEntityRef GetEntity(uint32_t p_Index)
    {
        if (m_bDelayedCreateEntities && m_bReady && m_iEntitiesNUM != m_Entities.Size())
        {
            CreateEntities();
        }

        if (m_Entities.Size() > 0 && p_Index < m_Entities.Size())
        {
            return m_Entities[p_Index];
        }

        return ZEntityRef{};
    }

    void WaitForResources()
    {
        while (!IsReady())
        {
            Globals::ResourceManager->Update(true);
            ZThread::Sleep(20);
        }
    }

    bool IsReady()
    {
        if (m_bFailed)
        {
            return true;
        }

        return m_bReady;
    }

    bool IsFailed()
    {
        return m_bFailed;
    }

    void CreateEntities()
    {
        Functions::ZDynamicResourceLibrary_CreateEntities->Call(this);
    }

    void DeleteAllEntities()
    {
        Functions::ZDynamicResourceLibrary_DeleteAllEntities->Call(this);
    }

    void OnHeaderLibraryReady(const ZRuntimeResourceID& p_RuntimeResourceIDD)
    {
        Functions::ZDynamicResourceLibrary_OnHeaderLibraryReady->Call(this, p_RuntimeResourceIDD);
    }

    void OnResourceLibrariesReady()
    {
        Functions::ZDynamicResourceLibrary_OnResourceLibrariesReady->Call(this);
    }

    ZRuntimeResourceID m_ridHeaderLibrary;
    ZResourceLibrarySet* m_pCurrentLibrarySet;
    TResourcePtr<ZHeaderLibrary> m_HeaderLibrary;
    ZEvent<ZRuntimeResourceID const&, ZEventNull, ZEventNull, ZEventNull, ZEventNull> m_LibraryReadyEvent;
    TArray<ZEntityRef> m_Entities;
    const uint32_t m_iEntitiesNUM;
    bool m_bReady : 1;
    bool m_bFailed : 1;
    bool m_bLoadingHeader : 1;
    bool m_bDelayedCreateEntities : 1;
};
