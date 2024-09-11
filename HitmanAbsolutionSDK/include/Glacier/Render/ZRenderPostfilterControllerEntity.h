#pragma once

#include "ZRenderableEntity.h"
#include "IRenderPostfilterControllerEntity.h"

class ZRenderPostfilterControllerEntity : public ZRenderableEntity, public IRenderPostfilterControllerEntity
{
private:
	ZEntityRef m_PostfilterParametersEntity; //0x64
	PAD(0x24);
	ZEntityRef m_CurrentPostfilterParametersEntity; //0x8C
};
