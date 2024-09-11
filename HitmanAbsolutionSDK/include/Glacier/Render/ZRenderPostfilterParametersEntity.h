#pragma once

#include "../Entity/ZEntityImpl.h"
#include "IRenderPostfilterParametersEntity.h"
#include "ICurveChanged.h"

class ZRenderPostfilterParametersEntity : public ZEntityImpl, public IRenderPostfilterParametersEntity, public ICurveChanged
{
};
