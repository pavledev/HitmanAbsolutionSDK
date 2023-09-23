#pragma once

#include "TRenderReferencedCountedImpl.h"

enum ERenderResourceType;

template <typename T, int U>
class TRenderResourceImpl : public TRenderReferencedCountedImpl<T, U>
{
public:
	virtual ~TRenderResourceImpl() = 0;
	virtual ERenderResourceType GetResourceType() const = 0;
};
