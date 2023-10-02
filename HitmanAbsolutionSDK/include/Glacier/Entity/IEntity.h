#pragma once

#include "../IComponentInterface.h"

#include <Common.h>

class ZEntityRef;
class ZString;
template <typename TKey, typename TValue> class THashSet;
template <typename T> class TDefaultHashSetPolicy;

class HitmanAbsolutionSDK_API IEntity : public IComponentInterface
{
public:
	virtual ~IEntity() = default;
	virtual ZEntityRef GetID() const = 0;
	virtual void Activate(const ZString& string) = 0;
	virtual void Deactivate(const ZString& string) = 0;
	virtual void StreamIn() = 0;
	virtual void StreamOut() = 0;
	virtual bool IsStreamedIn() = 0;
	virtual void GetStreamableIncludes(THashSet<ZEntityRef, TDefaultHashSetPolicy<ZEntityRef>>& hashSet) const = 0;
	virtual void OnEnterEditMode() = 0;
	virtual void OnExitEditMode() = 0;
};
