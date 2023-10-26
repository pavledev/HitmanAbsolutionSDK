#include <Glacier/Action/ZHM5Action.h>

EActionType ZHM5Action::GetActionType() const
{
	return m_eActionType;
}

TEntityRef<IEntity> ZHM5Action::GetActionObject() const
{
	return m_Object;
}
