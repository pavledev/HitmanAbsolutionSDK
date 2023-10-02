#pragma once

#include "../IComponentInterface.h"

class ZEntityRef;
class IEntityFactory;
class ZString;
class IEntityBlueprintFactory;
template <typename T> class TResourcePtr;
class ZResourcePtr;
class ZEventNull;
template <typename A, typename B, typename C, typename D, typename E> class ZEvent;
template <typename T> class ZDelegate;
template <typename T> class TArrayRef;

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
	virtual void SetSceneResources(TResourcePtr<IEntityFactory> resourcePtr, TResourcePtr<IEntityBlueprintFactory> resourcePtr2, ZResourcePtr resourcePtr3) = 0;
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
