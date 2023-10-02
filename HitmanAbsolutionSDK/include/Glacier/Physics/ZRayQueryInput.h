#pragma once

#include "../Math/float4.h"
#include "ECollidablesType.h"
#include "ERayDetailLevel.h"
#include "../ZDelegate.h"

#include <Common.h>

class ZEntityRef;
template <typename T> class TEntityRef;
class ZSpatialEntity;

class HitmanAbsolutionSDK_API ZRayQueryInput
{
public:
	ZRayQueryInput(const float4& from, const float4& to, const ERayDetailLevel rayDetailLevel);

private:
	float4 m_vFrom;
	float4 m_vTo;
	unsigned int m_nRayFilter;
	ECollidablesType m_eType;
	ERayDetailLevel m_eRayDetailLevel;
	ZDelegate<bool __cdecl(ZEntityRef, TEntityRef<ZSpatialEntity>)> m_FilterCallback;
	bool m_bIgnoreTransparentMaterials;
};
