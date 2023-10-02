#pragma once

#include "../Math/float4.h"
#include "../Templates/TResourcePtr.h"

#include <Common.h>

class ZRenderMaterialInstance;

class HitmanAbsolutionSDK_API ZRayQueryOutputBase
{
public:
	const float4& GetPosition() const;

protected:
	float4 m_vPosition;
	float4 m_vNormal;
	float m_nT;
	TResourcePtr<ZRenderMaterialInstance> m_pBlockingMaterial;
	unsigned char m_nIntersectedBoneMeshId;
	bool m_bHasHit;
};
