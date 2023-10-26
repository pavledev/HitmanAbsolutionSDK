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
	TArray<TEntityRef<IHM5Item>>& GetRuntimeInventory();
	void KillActor(EActorDeathType eDeathType, bool bDeathVisible);
	void SetCCProfile(TEntityRef<ZHM5CCProfile> ccProfile);
	void SetActorName(const ZString& actorName);

private:
	TEntityRef<ZHM5CCProfile> m_rCCProfile; //0x1F4
	PAD(0x10);
	ZString m_sActorName; //0x20C
	EActorType m_eActorType; //0x214
	PAD(0x1C);
	TArray<TEntityRef<IHM5Item>> m_inventory; //0x234
	PAD(0x64);
	TEntityRef<ZCharacterTemplateAspect> m_pCharacterTemplate; //0x2A4
	PAD(0x910);
	TArray<TEntityRef<IHM5Item>> m_runtimeInventory; //0xBBC
	PAD(0x38);
};

static_assert(sizeof(ZActor) == 0xC00);
