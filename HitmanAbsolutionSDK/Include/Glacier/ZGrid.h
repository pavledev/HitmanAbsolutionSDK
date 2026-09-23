#pragma once

#include "ZMath.h"
#include "IComponentInterface.h"
#include "TArray.h"
#include "ZBitArray.h"

class ZGridByteField;
class ZAIGridHitmanMappingVolume;
class ZDisableGridArea;

struct SGProperties
{
    float4 vMin;
    float4 vMax;
    int32_t nGridWidth;
    float fGridSpacing;
};

struct SGWaypoint
{
    union
    {
        struct
        {
            int16_t nNeighbor0;
            int16_t nNeighbor1;
            int16_t nNeighbor2;
            int16_t nNeighbor3;
            int16_t nNeighbor4;
            int16_t nNeighbor5;
            int16_t nNeighbor6;
            int16_t nNeighbor7;
        };

        int16_t Neighbors[8];
    };

    float4 vPos;
};

struct SReasoningGrid
{
    const SGWaypoint* GetNode(uint16_t p_Index) const
    {
        return &m_WaypointList[p_Index];
    }

    bool HasVisibilityLow(uint32_t p_FromNode, uint32_t p_ToNode) const
    {
        if (p_FromNode == p_ToNode)
        {
            return true;
        }

        const uint32_t bitIndex = p_ToNode + p_FromNode * m_nNodeCount;

        return m_LowVisibilityBits.Get(bitIndex);
    }

    bool HasVisibilityHigh(uint32_t p_FromNode, uint32_t p_ToNode) const
    {
        if (p_FromNode == p_ToNode)
        {
            return true;
        }

        if (p_ToNode < p_FromNode)
        {
            std::swap(p_FromNode, p_ToNode);
        }

        const uint32_t bitIndex = p_FromNode * (m_nNodeCount - 1) - ((p_FromNode * (p_FromNode + 1)) >> 1) + p_ToNode - 1;

        return m_HighVisibilityBits.Get(bitIndex);
    }

    TArray<SGWaypoint> m_WaypointList;
    ZBitArray m_LowVisibilityBits;
    ZBitArray m_HighVisibilityBits;
    TArray<TArray<uint16_t>> m_LowVisibility;
    TArray<TArray<uint16_t>> m_HighVisibility;
    SGProperties m_Properties;
    uint32_t m_BitVisibilityCount;
    uint32_t m_nNodeCount;
};

class ZGridManager : public IComponentInterface
{
  public:
    static float4 CellToPosition(int32_t p_X, int32_t p_Y, float p_Z, const SGProperties& p_Properties)
    {
        return float4(
            static_cast<float>(p_X) * p_Properties.fGridSpacing + p_Properties.vMin.x,
            static_cast<float>(p_Y) * p_Properties.fGridSpacing + p_Properties.vMin.y, p_Z, 0.f
        );
    }

    static float4 GetCellUpperLeft(const float4& p_Position, const SGProperties& p_Properties)
    {
        const float4 vertexPosition = p_Position - p_Properties.vMin;
        const int32_t x = static_cast<int32_t>(vertexPosition.x * (1.f / p_Properties.fGridSpacing));
        const int32_t y = static_cast<int32_t>(vertexPosition.y * (1.f / p_Properties.fGridSpacing));

        return CellToPosition(x, y, p_Position.z, p_Properties);
    }

    static uint32_t GetHeatmapColorFromRating(float p_Rating)
    {
        if (p_Rating < 0.0f)
        {
            return 0xFF000000;
        }

        if (p_Rating > 1.0f)
        {
            return 0xFFFFFFFF;
        }

        if (p_Rating < 0.5f)
        {
            int32_t color = static_cast<int32_t>((p_Rating * 2.0f) * 255.0f);
            color = std::clamp(color, 0, 255);

            return ((color << 8) - 0xFFFF01);
        }
        else
        {
            int32_t color = static_cast<int32_t>((1.0f - (p_Rating - 0.5f) * 2.0f) * 255.0f);
            color = std::clamp(color, 0, 255);

            return color - 0xFF0100;
        }
    }
};

class ZGridNodeRef
{
  public:
    ZGridNodeRef(uint16_t p_NodeIndex)
    {
        m_nNodeIndex = p_NodeIndex;
        m_nRoomID = -1;
        m_pNode = (*Globals::ActiveGrid)->GetNode(p_NodeIndex);
    }

    bool CheckVisibility(const ZGridNodeRef& p_Other, bool p_Low, bool p_CheckDoors) const
    {
        if (!m_pNode || !p_Other.m_pNode)
        {
            return false;
        }

        uint32_t fromNodeIndex = m_nNodeIndex;
        uint32_t toNodeIndex = p_Other.m_nNodeIndex;

        if (p_Low)
        {
            return (*Globals::ActiveGrid)->HasVisibilityLow(fromNodeIndex, toNodeIndex);
        }

        return (*Globals::ActiveGrid)->HasVisibilityHigh(fromNodeIndex, toNodeIndex);
    }

    uint16_t m_nNodeIndex;
    const SGWaypoint* m_pNode;
    uint16_t m_nRoomID;
};

class ZHM5GridManager : public IComponentInterface
{
  public:
    PAD(0x1C);                 // 0x4
    ZGridNodeRef m_HitmanNode; // 0x20
};
