#pragma once

#include "IComponentInterface.h"
#include "ZResource.h"
#include "TArray.h"
#include "ZEntity.h"

template<typename T> class TResourcePtr;
class ZResourcePtr;
class ZEventNull;
template<typename A, typename B, typename C, typename D, typename E> class ZEvent;
template<typename T> class ZDelegate;
template<typename T> class TArrayRef;

class IEntitySceneContext : public IComponentInterface
{
  public:
    virtual ~IEntitySceneContext() = default;
    virtual void AddEntity(const ZEntityRef& entityRef, IEntityFactory* entityFactory) = 0;
    virtual void RemoveEntity(const ZEntityRef& entityRef) = 0;
    virtual void ClearScene(bool param1) = 0;
    virtual void PrepareNewScene() = 0;
    virtual void CreateScene() = 0;
    virtual void CreateScene(const ZString& string) = 0;
    virtual void ResetScene(const ZString& string) = 0;
    virtual void
    SetSceneResources(TResourcePtr<IEntityFactory> resourcePtr, TResourcePtr<IEntityBlueprintFactory> resourcePtr2, ZResourcePtr resourcePtr3) = 0;
    virtual void DropSceneFactory() = 0;
    virtual bool IsRestartingThroughEngine() const = 0;
    virtual ZEvent<ZEntityRef const&, ZEventNull, ZEventNull, ZEventNull, ZEventNull>& GetEntityAddedEvent() = 0;
    virtual ZEvent<ZEntityRef const&, ZEventNull, ZEventNull, ZEventNull, ZEventNull>& GetEntityRemovedEvent() = 0;
    virtual ZEvent<ZEventNull, ZEventNull, ZEventNull, ZEventNull, ZEventNull>& GetRequestSceneResetEvent() = 0;
    virtual ZResourcePtr GetSceneHeaderLibrary() const = 0;
    virtual bool IsClearingGraph() const = 0;
    virtual void RegisterForResetSceneEvent(const ZDelegate<void __cdecl(void)>& delegate) = 0;
    virtual void UnregisterForResetSceneEvent(const ZDelegate<void __cdecl(void)>& delegate) = 0;
    virtual ZEntityRef GetScene() = 0;
    virtual void StartEntities() = 0;
    virtual ZEntityRef CreateUninitializedEntityWithFactory(const ZString& string, IEntityFactory* entityFactory) = 0;
    virtual void DeleteEntities(const TArrayRef<ZEntityRef> arrayRef) = 0;
};

class ISceneEntity : public IComponentInterface
{};

struct SSceneInitParameters
{
    ZString m_SceneResource;
    ZString m_StreamingState;
};

class ZEntitySceneContext : public IEntitySceneContext
{
  public:
    virtual void AddEntity(const ZEntityRef& entity, IEntityFactory* pFactory) = 0;
    virtual void RemoveEntity(const ZEntityRef& entity) = 0;
    virtual void ClearScene(bool bFullyUnloadScene) = 0;
    virtual void PrepareNewScene() = 0;
    virtual void CreateScene() = 0;
    virtual void CreateScene(const ZString& sStreamingState) = 0;
    virtual void ResetScene(const ZString& sStreamingState) = 0;
    virtual void SetSceneResources(
        TResourcePtr<IEntityFactory> pSceneFactoryResource, TResourcePtr<IEntityBlueprintFactory> pSceneBlueprintResource,
        ZResourcePtr pSceneHeaderLibraryResource
    ) = 0;
    virtual void DropSceneFactory() = 0;
    virtual bool IsRestartingThroughEngine() const = 0;
    virtual ZEvent<ZEntityRef const&, ZEventNull, ZEventNull, ZEventNull, ZEventNull>& GetEntityAddedEvent() = 0;
    virtual ZEvent<ZEntityRef const&, ZEventNull, ZEventNull, ZEventNull, ZEventNull>& GetEntityRemovedEvent() = 0;
    virtual ZEvent<ZEventNull, ZEventNull, ZEventNull, ZEventNull, ZEventNull>& GetRequestSceneResetEvent() = 0;
    virtual ZResourcePtr GetSceneHeaderLibrary() const = 0;
    virtual bool IsClearingGraph() const = 0;
    virtual void RegisterForResetSceneEvent(const ZDelegate<void __cdecl(void)>& fpCallback) = 0;
    virtual void UnregisterForResetSceneEvent(const ZDelegate<void __cdecl(void)>& fpCallback) = 0;
    virtual ZEntityRef GetScene() = 0;
    virtual void StartEntities() = 0;
    virtual ZEntityRef CreateUninitializedEntityWithFactory(const ZString& sDebugName, IEntityFactory* pEntityFactory) = 0;
    virtual void DeleteEntities(const TArrayRef<ZEntityRef> aEntities) = 0;
    virtual void ResetSceneResources() = 0;
    virtual void SetSceneInitParameters(const SSceneInitParameters& params) = 0;
    virtual bool IsLoaded() = 0;

    ZRuntimeResourceID m_ridSceneFactory;
    TResourcePtr<IEntityFactory> m_sceneFactoryResource;
    TResourcePtr<IEntityBlueprintFactory> m_sceneBlueprintResource;
    uint8_t* m_pSceneMemoryBlock;
    uint32_t m_nSceneMemoryBlockSize;
    ZResourcePtr m_pSceneHeaderLibraryResource;
    ZEntityRef m_pLoadedScene;
    bool m_bRestartingThroughEngine;
    PAD(0x33);
    SSceneInitParameters m_SceneInitParameters;
    bool m_bIsClearingGraph;
    TEntityRef<ISceneEntity> m_pScene;
    TArray<ZEntityRef> m_EntitiesOwnedByTheScene;
    PAD(0x10);
};
