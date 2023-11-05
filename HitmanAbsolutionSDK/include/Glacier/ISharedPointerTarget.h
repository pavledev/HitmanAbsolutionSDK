#pragma once

#include <Common.h>

class HitmanAbsolutionSDK_API ISharedPointerTarget
{
public:
	virtual ~ISharedPointerTarget() = default;
	virtual void AddReference() const = 0;
	virtual void RemoveReference() const = 0;
	virtual unsigned int GetRefCount() const = 0;
};
