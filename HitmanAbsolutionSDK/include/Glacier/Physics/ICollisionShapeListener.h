#pragma once

#include "../IComponentInterface.h"

class ICollisionShapeListener : public IComponentInterface
{
public:
	virtual ~ICollisionShapeListener() = 0;
	virtual void OnCollisionShapeChanged() = 0;
};
