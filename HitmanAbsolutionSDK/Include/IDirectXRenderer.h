#pragma once

#include "Glacier/ZMath.h"

class ZRenderPrimitiveResource;
class ZRenderVertexBuffer;
class ZRenderIndexBuffer;

struct Line
{
    SVector3 m_Start;
    SVector4 m_StartColor;
    SVector3 m_End;
    SVector4 m_EndColor;
};

struct Vertex
{
    SVector3 m_Position;
    SVector4 m_Color;
    SVector2 m_TexCoord;
};

struct Triangle
{
    Vertex m_Vertex1;
    Vertex m_Vertex2;
    Vertex m_Vertex3;
};

struct AABB
{
    SVector3 m_Min;
    SVector3 m_Max;
};

enum class HorizontalAlignment
{
    Left,
    Center,
    Right
};

enum class VerticalAlignment
{
    Top,
    Middle,
    Bottom
};

class IDirectXRenderer
{
  public:
    virtual ~IDirectXRenderer() = default;

    virtual bool WorldToScreen(const SVector3& p_WorldPosition, SVector2& p_OutScreenPosition) = 0;
    virtual bool ScreenToWorld(const SVector2& p_ScreenPosition, SVector3& p_OutWorldPosition, SVector3& p_OutDirection) = 0;

    virtual void DrawLine3D(const SVector3& p_From, const SVector4& p_FromColor, const SVector3& p_To, const SVector4& p_ToColor) = 0;

    virtual void DrawBox3D(const SVector3& p_Min, const SVector3& p_Max, const SVector4& p_Color) = 0;

    virtual void DrawBox3D(const SVector3& p_Center, const SVector3& p_Size, const SMatrix& p_Transform, const SVector4& p_Color) = 0;

    virtual void DrawBoxWire3D(const SVector3& p_Center, const SVector3& p_Size, const SMatrix& p_Transform, const SVector4& p_Color) = 0;

    virtual void DrawOBB3D(const SVector3& p_Min, const SVector3& p_Max, const SMatrix& p_Transform, const SVector4& p_Color) = 0;

    virtual void DrawBoundingQuads3D(const SVector3& p_Min, const SVector3& p_Max, const SMatrix& p_Transform, const SVector4& p_Color) = 0;

    virtual void DrawTriangle3D(
        const SVector3& p_V1, const SVector4& p_Color1, const SVector3& p_V2, const SVector4& p_Color2, const SVector3& p_V3, const SVector4& p_Color3
    ) = 0;

    virtual void DrawTriangle3D(
        const SVector3& p_V1, const SVector4& p_Color1, const SVector2& p_TextureCoordinates1, const SVector3& p_V2, const SVector4& p_Color2,
        const SVector2& p_TextureCoordinates2, const SVector3& p_V3, const SVector4& p_Color3, const SVector2& p_TextureCoordinates3
    ) = 0;

    virtual void DrawQuad3D(
        const SVector3& p_V1, const SVector4& p_Color1, const SVector3& p_V2, const SVector4& p_Color2, const SVector3& p_V3,
        const SVector4& p_Color3, const SVector3& p_V4, const SVector4& p_Color4
    ) = 0;

    virtual void DrawMesh(const std::vector<SVector3>& p_Vertices, const std::vector<unsigned short>& p_Indices, const SVector4& p_VertexColor) = 0;

    virtual void DrawMesh(
        ZRenderPrimitiveResource* p_RenderPrimitiveResource, ZRenderVertexBuffer** p_VertexBuffers, uint32_t p_VertexBufferCount,
        ZRenderIndexBuffer* p_IndexBuffer, const SMatrix& p_Transform, const float4& p_PositionScale, const float4& p_PositionBias,
        const float4& p_TextureScaleBias, const SVector4& p_MaterialColor
    ) = 0;

    virtual void DrawText2D(
        const ZString& p_Text, const SVector2& p_Position, const SVector4& p_Color, float p_Rotation = 0.f, float p_Scale = 1.f,
        HorizontalAlignment p_HorizontalAlignment = HorizontalAlignment::Center, VerticalAlignment p_VerticalAlignment = VerticalAlignment::Top
    ) = 0;

    virtual void DrawText3D(
        const char* p_Text, const SMatrix& p_World, const SVector4& p_Color, float p_Scale = 1.f,
        HorizontalAlignment p_HorizontalAlignment = HorizontalAlignment::Left, VerticalAlignment p_VerticalAlignment = VerticalAlignment::Top,
        bool p_IsCameraTransform = true
    ) = 0;

    virtual bool IsPointInsideViewFrustum(const SVector3& p_Point) const = 0;

    virtual bool IsAABBInsideViewFrustum(const SVector3& p_Min, const SVector3& p_Max, const SMatrix& p_Transform) const = 0;

    virtual bool IsOBBInsideViewFrustum(const float4& p_Center, const float4& p_HalfSize, const SMatrix& p_Transform) const = 0;

    virtual bool IsSphereInsideViewFrustum(const SVector3& p_Center, float p_Radius) const = 0;

    virtual void SetFrustumCullingEnabled(bool p_Enabled) = 0;
    virtual bool IsFrustumCullingEnabled() const = 0;

    virtual void SetDistanceCullingEnabled(bool p_Enabled) = 0;
    virtual bool IsDistanceCullingEnabled() const = 0;

    virtual void SetMaxDrawDistance(float p_MaxDrawDistance) = 0;
    virtual float GetMaxDrawDistance() const = 0;
};
