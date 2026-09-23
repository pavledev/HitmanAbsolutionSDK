#pragma once

#include "TArray.h"
#include "ZMath.h"
#include "ZDelegate.h"

enum ERegionMask
{
    eRM_None = 1,
    eRM_LowCover = 2,
    eRM_HighCover = 4,
    eRM_Stairs = 8,
    eRM_Separator = 16,
    eRM_User_1 = 32,
    eRM_User_2 = 64,
    eRM_User_3 = 128,
    eRM_User_4 = 256,
    eRM_User_5 = 512,
    eRM_User_6 = 1024,
    eRM_User_7 = 2048,
    eRM_User_8 = 4096
};

class ZNavmesh
{
  public:
    uint32_t GetCornerCount(uint32_t p_PolygonID) const
    {
        return m_aPolygonOffsets[p_PolygonID + 1] - m_aPolygonOffsets[p_PolygonID];
    }

    uint16_t GetAnnotation(uint32_t p_PolygonID) const
    {
        return m_aAnnotation[p_PolygonID];
    }

    uint32_t GetVertexID(uint32_t p_PolygonID, uint32_t p_CornerID) const
    {
        return m_aPolygonData[2 * m_aPolygonOffsets[p_PolygonID] + p_CornerID];
    }

    float4 GetVertex(uint32_t p_VertexID) const
    {
        const SVector3& vertex = m_aVertices[p_VertexID];

        return float4(vertex.x, vertex.y, vertex.z, vertex.z);
    }

    uint32_t GetPolygonVertices(uint32_t p_PolygonID, uint32_t p_MaxVertices, float4* p_Vertices) const
    {
        uint32_t vertexCount = GetCornerCount(p_PolygonID);

        if (vertexCount > p_MaxVertices)
        {
            vertexCount = p_MaxVertices;
        }

        for (uint32_t i = 0; i < vertexCount; ++i)
        {
            const uint32_t vertexID = GetVertexID(p_PolygonID, i);

            p_Vertices[i] = GetVertex(vertexID);
        }

        return vertexCount;
    }

    uint32_t m_nPolygonCount;
    uint32_t m_nCornerCount;
    uint32_t m_nVertexCount;
    uint32_t m_nPlaneCount;
    const uint16_t* m_aPolygonOffsets;
    const uint16_t* m_aPolygonData;
    const SVector3* m_aVertices;
    const uint16_t* m_aAnnotation;
    const uint16_t* m_aPlaneIndices;
    const SVector4* m_aPlaneEquations;
    SVector2 m_vBoundsMin;
    SVector2 m_vBoundsMax;
    float m_fTileSize;
    int32_t m_nTileCountX;
    int32_t m_nTileCountY;
    TArray<TArray<uint16_t>> m_tiles;
    TArray<float> m_aCostMultipliers;
    TArray<ZDelegate<void __cdecl(ERegionMask, float, float)>> m_aRegionCostListeners;
};

class ZPathfinder2 : public IComponentInterface
{
  public:
    PAD(0xF454);          // 0x4
    ZNavmesh* m_pNavmesh; // 0xF458
};
