#pragma once

#include "../ZHM5BaseCharacter.h"
#include "../Physics/ICharacterCollision.h"
#include "EActorDeathType.h"
#include "../ZCharacterTemplateAspect.h"
#include "EActorType.h"
#include "ZHM5CCProfile.h"

class HitmanAbsolutionSDK_API ZActor : public ZHM5BaseCharacter, public ICharacterCollision
{
public:
	const ZString& GetActorName() const;
	ZRuntimeResourceID GetHMAsResourceID() const;
	void KillActor(EActorDeathType eDeathType, bool bDeathVisible);
	void SetCCProfile(TEntityRef<ZHM5CCProfile> ccProfile);
	void SetActorName(const ZString& actorName);

private:
	TEntityRef<ZHM5CCProfile> m_rCCProfile; //0x1F4
	PAD(0x10);
	ZString m_sActorName; //0x20C
	EActorType m_eActorType; //0x214
	PAD(0x8C);
	TEntityRef<ZCharacterTemplateAspect> m_pCharacterTemplate; //0x2A4
	PAD(0x954);
};

static_assert(sizeof(ZActor) == 0xC00);
