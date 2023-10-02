#include <Glacier/Physics/ZRayQueryInput.h>

ZRayQueryInput::ZRayQueryInput(const float4& from, const float4& to, const ERayDetailLevel rayDetailLevel)
{
	m_vFrom = from;
	m_vTo = to;
	m_eRayDetailLevel = rayDetailLevel;
	m_nRayFilter = 0;
	m_eType = ECollidablesType::ECST_STATIC_AND_DYNAMIC;
	m_bIgnoreTransparentMaterials = false;
}
