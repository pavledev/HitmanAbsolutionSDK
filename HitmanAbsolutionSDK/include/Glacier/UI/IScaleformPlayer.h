#pragma once

#include "../IComponentInterface.h"
#include "GFxValue.h"

class IScaleformPlayer : public IComponentInterface
{
public:
	virtual void IScaleformPlayer_unk5() = 0;
	virtual void IScaleformPlayer_unk6() = 0;
	virtual void IScaleformPlayer_unk7() = 0;
	virtual void IScaleformPlayer_unk8() = 0;
	virtual void IScaleformPlayer_unk9() = 0;
	virtual void IScaleformPlayer_unk10() = 0;
	virtual void IScaleformPlayer_unk11() = 0;
	virtual void IScaleformPlayer_unk12() = 0;
	virtual void IScaleformPlayer_unk13() = 0;
	virtual void IScaleformPlayer_unk14() = 0;
	virtual void IScaleformPlayer_unk15() = 0;
	virtual bool GetMember(const char* param1, GFxValue* fxValue) = 0;
};
