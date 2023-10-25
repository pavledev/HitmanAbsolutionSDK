#pragma once

#include "../IComponentInterface.h"
#include "../Entity/ZEntityRef.h"
#include "../STokenID.h"

class IHM5Outfit : public IComponentInterface
{
public:
	virtual bool IsAllowedIn(const ZEntityRef& entityRef) const = 0;
	virtual int GetZoneMask() const = 0;
	virtual bool SupportsActor(const STokenID& tokenID) = 0;
};
