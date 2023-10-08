#include <Glacier/ContentKit/ZContentKitEntity.h>

const STokenID& ZContentKitEntity::GetTokenID() const
{
	ZTokenIDEntity* tokenIDEntity = m_Token.GetRawPointer();

	if (tokenIDEntity)
	{
		return tokenIDEntity->GetTokenID();
	}

	return STokenID::InvalidToken;
}

const ZString& ZContentKitEntity::GetLocalizedTitle() const
{
	return m_sCachedTitle;
}

const ZString& ZContentKitEntity::GetLocalizedDescription() const
{
	return m_sCachedDescription;
}
