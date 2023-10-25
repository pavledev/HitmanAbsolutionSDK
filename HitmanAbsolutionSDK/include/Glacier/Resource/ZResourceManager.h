#pragma once

#include "../IComponentInterface.h"
#include "ZResourcePtr.h"
#include "ZRuntimeResourceID.h"

class HitmanAbsolutionSDK_API ZResourceManager : public IComponentInterface
{
public:
	ZResourcePtr GetResourcePtr(const ZRuntimeResourceID& ridResource, int nPriority);
	ZResourcePtr LoadResource(const ZRuntimeResourceID& ridResource);
	void Update(bool bSendStatusChangedNotifications);
	void ReleaseStub(ZResourceStub* stub);
};
