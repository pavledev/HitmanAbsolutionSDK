#pragma once

#include <Windows.h>

#include "directxtk/Effects.h"
#include "directxtk/PrimitiveBatch.h"
#include "directxtk/SimpleMath.h"
#include "directxtk/SpriteBatch.h"
#include "directxtk/VertexTypes.h"
#include "directxtk/SpriteFont.h"
#include "directxtk/CommonStates.h"

#include "Glacier/ZMath.h"
#include "Glacier/ZString.h"
#include "Glacier/ZRender.h"

#include "IDirectXRenderer.h"
#include "ViewFrustum.h"
#include "DebugEffect.h"
#include "CustomPrimitiveBatch.h"
#include "Hooks.h"

class DirectXRenderer final : public IDirectXRenderer
{
  public:
    ~DirectXRenderer() override;

    void TeardownRenderer();

    void OnEngineInitialized();
    void OnPresent(ZRenderDevice* p_RenderDevice);
    void OnResize(const SRenderDestinationDesc* p_Description);

    bool WorldToScreen(const SVector3& p_WorldPosition, SVector2& p_OutScreenPosition) override;
    bool ScreenToWorld(const SVector2& p_ScreenPosition, SVector3& p_OutWorldPosition, SVector3& p_OutDirection) override;

    void DrawLine3D(const SVector3& p_From, const SVector4& p_FromColor, const SVector3& p_To, const SVector4& p_ToColor) override;

    void DrawBox3D(const SVector3& p_Min, const SVector3& p_Max, const SVector4& p_Color) override;
    void DrawBox3D(const SVector3& p_Center, const SVector3& p_Size, const SMatrix& p_Transform, const SVector4& p_Color) override;
    void DrawBoxWire3D(const SVector3& p_Center, const SVector3& p_Size, const SMatrix& p_Transform, const SVector4& p_Color) override;

    void DrawOBB3D(const SVector3& p_Min, const SVector3& p_Max, const SMatrix& p_Transform, const SVector4& p_Color) override;

    void DrawBoundingQuads3D(const SVector3& p_Min, const SVector3& p_Max, const SMatrix& p_Transform, const SVector4& p_Color) override;

    void DrawTriangle3D(
        const SVector3& p_V1, const SVector4& p_Color1, const SVector3& p_V2, const SVector4& p_Color2, const SVector3& p_V3, const SVector4& p_Color3
    ) override;

    void DrawTriangle3D(
        const SVector3& p_V1, const SVector4& p_Color1, const SVector2& p_TextureCoordinates1, const SVector3& p_V2, const SVector4& p_Color2,
        const SVector2& p_TextureCoordinates2, const SVector3& p_V3, const SVector4& p_Color3, const SVector2& p_TextureCoordinates3
    ) override;

    void DrawQuad3D(
        const SVector3& p_V1, const SVector4& p_Color1, const SVector3& p_V2, const SVector4& p_Color2, const SVector3& p_V3,
        const SVector4& p_Color3, const SVector3& p_V4, const SVector4& p_Color4
    ) override;

    void DrawText2D(
        const ZString& p_Text, const SVector2& p_Position, const SVector4& p_Color, float p_Rotation = 0.f, float p_Scale = 1.f,
        HorizontalAlignment p_HorizontalAlignment = HorizontalAlignment::Center, VerticalAlignment p_VerticalAlignment = VerticalAlignment::Top
    ) override;

    void DrawText3D(
        const char* p_Text, const SMatrix& p_Transform, const SVector4& p_Color, float p_Scale = 1.f,
        HorizontalAlignment p_HorizontalAlignment = HorizontalAlignment::Left, VerticalAlignment p_VerticalAlignment = VerticalAlignment::Top,
        bool p_IsCameraTransform = true
    ) override;

    void DrawMesh(const std::vector<SVector3>& p_Vertices, const std::vector<uint16_t>& p_Indices, const SVector4& p_VertexColor) override;

    void DrawMesh(
        ZRenderPrimitiveResource* p_RenderPrimitiveResource, ZRenderVertexBuffer** p_VertexBuffers, uint32_t p_VertexBufferCount,
        ZRenderIndexBuffer* p_IndexBuffer, const SMatrix& p_Transform, const float4& p_PositionScale, const float4& p_PositionBias,
        const float4& p_TextureScaleBias, const SVector4& p_MaterialColor
    ) override;

    bool IsPointInsideViewFrustum(const SVector3& p_Point) const override;
    bool IsAABBInsideViewFrustum(const SVector3& p_Min, const SVector3& p_Max, const SMatrix& p_Transform) const override;
    bool IsOBBInsideViewFrustum(const float4& p_Center, const float4& p_HalfSize, const SMatrix& p_Transform) const override;
    bool IsSphereInsideViewFrustum(const SVector3& p_Center, float p_Radius) const override;

    void SetFrustumCullingEnabled(bool p_Enabled) override;
    bool IsFrustumCullingEnabled() const override;

    void SetDistanceCullingEnabled(bool p_Enabled) override;
    bool IsDistanceCullingEnabled() const override;

    void SetMaxDrawDistance(float p_MaxDrawDistance) override;
    float GetMaxDrawDistance() const override;

  private:
    bool SetupRenderer();

    void Draw(ZRenderTargetView* p_RenderTargetView);
    void DepthDraw(ZRenderTargetView* p_RenderTargetView, ZRenderDepthStencilView* p_RenderDepthStencilView);

    bool CompileShaderFromString(
        const std::string& p_ShaderCode, const std::string& p_EntryPoint, const std::string& p_ShaderModel, ID3DBlob** p_ShaderBlob
    );

    bool CreateFontDistanceFieldTexture();

    static AABB TransformAABB(const DirectX::SimpleMath::Matrix& p_Transform, const AABB& p_AABB);

    DECLARE_THISCALL_DETOUR_WITH_CONTEXT(
        DirectXRenderer, void, ZRenderGraphNodeRenderer_DrawOSDAndScaleform, ZRenderGraphNodeRenderer* p_RenderGraphNodeRenderer,
        ZRenderContext* p_RenderContext, ZRenderTargetView* p_RTV, ZRenderDepthStencilView* p_DSV, TArray<ZRenderGraphNode*>& p_OSDNodes,
        ZRenderGraphTraversal* p_RenderGraphTraversal
    );

    bool m_IsRendererSetup = false;

    HWND m_Hwnd = nullptr;
    float m_WindowWidth = 1.f;
    float m_WindowHeight = 1.f;

    std::unique_ptr<DirectX::BasicEffect> m_PrimitiveEffect{};
    std::unique_ptr<DirectX::BasicEffect> m_TextEffect{};
    std::unique_ptr<DebugEffect> m_DebugEffect{};
    std::unique_ptr<CustomPrimitiveBatch<DirectX::VertexPositionColor>> m_PrimitiveBatch{};
    std::unique_ptr<CustomPrimitiveBatch<DirectX::VertexPositionColorTexture>> m_TextBatch{};
    ScopedD3DRef<ID3D11InputLayout> m_PrimitiveInputLayout;
    ScopedD3DRef<ID3D11InputLayout> m_TextInputLayout;

    ScopedD3DRef<ID3D11Texture2D> m_FontDistanceFieldTexture;
    ScopedD3DRef<ID3D11ShaderResourceView> m_FontDistanceFieldSRV;
    ScopedD3DRef<ID3D11VertexShader> m_FontVertexShader;
    ScopedD3DRef<ID3D11PixelShader> m_FontPixelShader;

    DirectX::SimpleMath::Matrix m_World{};
    DirectX::SimpleMath::Matrix m_View{};
    DirectX::SimpleMath::Matrix m_Projection{};
    DirectX::SimpleMath::Matrix m_ViewProjection{};
    DirectX::SimpleMath::Matrix m_ProjectionViewInverse{};

    std::unique_ptr<DirectX::SpriteFont> m_Font{};
    std::unique_ptr<DirectX::SpriteBatch> m_SpriteBatch{};

    ScopedD3DRef<ID3D11RasterizerState> m_CullNone;
    ScopedD3DRef<ID3D11RasterizerState> m_CullBack;
    ScopedD3DRef<ID3D11BlendState> m_AlphaBlend;
    ScopedD3DRef<ID3D11SamplerState> m_LinearClamp;
    ScopedD3DRef<ID3D11DepthStencilState> m_DepthDefault;
    ScopedD3DRef<ID3D11DepthStencilState> m_DepthRead;

    ViewFrustum m_ViewFrustum;
    bool m_IsFrustumCullingEnabled = true;
};
