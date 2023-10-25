#pragma once

#include "../ZMutex.h"
#include "../Templates/TMap.h"
#include "ZRuntimeResourceID.h"
#include "ZResourceID.h"

class LocalResourceIDsResolver
{
public:
	void RecordMapping(const ZRuntimeResourceID& ridResource, const ZResourceID& idResource);

private:
	ZMutex mutex;
	TMap<ZRuntimeResourceID, ZResourceID> runtimeResourceIDsToResourceIDs;
};
