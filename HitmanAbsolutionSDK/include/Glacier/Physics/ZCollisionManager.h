#pragma once

#include "NxUserContactReport.h"
#include "NxUserContactModify.h"

#include "ICollisionManager.h"
#include "ZRayQueryInput.h"
#include "ZRayQueryOutput.h"

class ZCollisionManager : public ICollisionManager, public NxUserContactReport, public NxUserContactModify
{
public:
	virtual void ZCollisionManager_Unk5() = 0;
	virtual void ZCollisionManager_Unk6() = 0;
	virtual void ZCollisionManager_Unk7() = 0;
	virtual void ZCollisionManager_Unk8() = 0;
	virtual void ZCollisionManager_Unk9() = 0;
	virtual void ZCollisionManager_Unk10() = 0;
	virtual void ZCollisionManager_Unk11() = 0;
	virtual void ZCollisionManager_Unk12() = 0;
	virtual void ZCollisionManager_Unk13() = 0;
	virtual void ZCollisionManager_Unk14() = 0;
	virtual void ZCollisionManager_Unk15() = 0;
	virtual bool RayCastClosestHit(const ZRayQueryInput& sInput, ZRayQueryOutput* pOutput) const = 0;
};
