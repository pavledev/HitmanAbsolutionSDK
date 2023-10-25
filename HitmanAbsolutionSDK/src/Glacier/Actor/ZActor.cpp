#include <Glacier/Actor/ZActor.h>

#include <Function.h>
#include <Global.h>

const ZString& ZActor::GetActorName() const
{
	return m_sActorName;
}

ZRuntimeResourceID ZActor::GetHMAsResourceID() const
{
	return m_pCharacterTemplate.GetRawPointer()->GetHMAsResourceID();
}

void ZActor::KillActor(EActorDeathType eDeathType, bool bDeathVisible)
{
	Function::CallMethod<ZActor*, EActorDeathType, bool>(BaseAddress + 0x174790, this, eDeathType, bDeathVisible);
}

void ZActor::SetCCProfile(TEntityRef<ZHM5CCProfile> ccProfile)
{
	m_rCCProfile = ccProfile;
}

void ZActor::SetActorName(const ZString& actorName)
{
	m_sActorName = actorName;
}
