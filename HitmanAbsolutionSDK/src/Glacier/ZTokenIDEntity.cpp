#include <Glacier/ZTokenIDEntity.h>

#include <Hash.h>
#include <Utility/StringUtility.h>

const STokenID& ZTokenIDEntity::GetTokenID()
{
    if (m_TokenIDCached == STokenID::InvalidToken)
    {
        std::string name = StringUtility::ToUpperCase(m_sName.ToCString());

        m_TokenIDCached.m_iValue = Hash::Crc32(name.c_str(), name.length());
        m_TokenIDCached.m_bValid = true;
    }

    return m_TokenIDCached;
}
