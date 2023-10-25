#include <Glacier/Actor/ZActorManager.h>

TArrayRef<TEntityRef<ZActor>> ZActorManager::GetAliveActors()
{
	return TArrayRef<TEntityRef<ZActor>>(reinterpret_cast<TEntityRef<ZActor>*>(m_aliveActors.m_data), m_aliveActors.Size());
}
