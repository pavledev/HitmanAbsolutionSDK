#pragma once

#include "ZHM5BaseCharacter.h"
#include "Physics/ICharacterCollision.h"
#include "EActorDeathType.h"

class HitmanAbsolutionSDK_API ZActor : public ZHM5BaseCharacter, public ICharacterCollision
{
public:
	void KillActor(EActorDeathType eDeathType, bool bDeathVisible);
};
