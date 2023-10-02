#pragma once

#include "../IComponentInterface.h"

class ZEntityRef;

class IRenderDestinationEntity : public IComponentInterface
{
public:
	virtual const ZEntityRef& GetSource() const = 0;
	virtual void IRenderDestinationEntity_Unk6() = 0;
	virtual void IRenderDestinationEntity_Unk7() const = 0;
	virtual void IRenderDestinationEntity_Unk8() const = 0;
	virtual void SetSource(const ZEntityRef& entityRef) = 0;
	virtual void IRenderDestinationEntity_Unk10(bool param1) = 0;
	virtual void IRenderDestinationEntity_Unk11() = 0;
};
