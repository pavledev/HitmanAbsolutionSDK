#pragma once

#include "IRenderRefCount.h"

enum ERenderResourceType;

class IRenderResource : public IRenderRefCount
{
public:
	virtual ~IRenderResource() = 0;
	virtual ERenderResourceType GetResourceType() const = 0;
};
