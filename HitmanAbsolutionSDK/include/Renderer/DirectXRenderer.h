#pragma once

#include <Windows.h>

#include "Glacier/ZMath.h"
#include "Glacier/ZString.h"
#include "Glacier/ZRender.h"

#include "directxtk/Effects.h"
#include "directxtk/PrimitiveBatch.h"
#include "directxtk/SimpleMath.h"
#include "directxtk/SpriteBatch.h"
#include "directxtk/VertexTypes.h"
#include "directxtk/SpriteFont.h"
#include "directxtk/CommonStates.h"

enum class TextAlignment
{
    Left,
    Center,
    Right,
};

class HitmanAbsolutionSDK_API DirectXRenderer
{
  public:
    DirectXRenderer();
    ~DirectXRenderer();

    bool Setup();
    void OnPresent(ZRenderDevice* p_RenderDevice);
    void OnResize(const SRenderDestinationDesc* p_Description);
    void Cleanup();
    void Render();

    void DrawLine3D(const SVector3& p_From, const SVector4& p_FromColor, const SVector3& p_To, const SVector4& p_ToColor);
    void DrawQuad3D(
        const SVector3& p_Vertex1, const SVector4& p_Color1, const SVector3& p_Vertex2, const SVector4& p_Color2, const SVector3& p_Vertex3,
        const SVector4& p_Color3, const SVector3& p_Vertex4, const SVector4& p_Color4
    );
    void DrawText2D(
        const ZString& p_Text, const SVector2& p_Position, const SVector4& p_Color, float p_Rotation = 0.f, float p_Scale = 1.f,
        TextAlignment p_Alignment = TextAlignment::Center
    );
    bool WorldToScreen(const SVector3& p_WorldPosition, SVector2& p_Out);
    bool ScreenToWorld(const SVector2& p_ScreenPosition, SVector3& p_OutWorldPosition, SVector3& p_OutDirection);
    void DrawBox3D(const SVector3& p_Min, const SVector3& p_Max, const SVector4& p_Color);
    void DrawOBB3D(const SVector3& p_Min, const SVector3& p_Max, const SMatrix& p_Transform, const SVector4& p_Color);

    static void CreateDDSTextureFromMemory(
        const void* data, const unsigned int resourceDataSize, ID3D11Resource** texture, ID3D11ShaderResourceView** textureView, float& width,
        float& height
    );
    static void LoadTextureFromFile(
        const char* textureFilePath, ID3D11Texture2D** texture, ID3D11ShaderResourceView** texureView, unsigned int& width, unsigned int& height
    );

  private:
    bool m_IsRendererSetup;

    HWND m_Hwnd;
    float m_WindowWidth;
    float m_WindowHeight;

    std::unique_ptr<DirectX::BasicEffect> m_LineEffect;
    std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_LineBatch;
    ID3D11InputLayout* m_InputLayout;

    DirectX::SimpleMath::Matrix m_World;
    DirectX::SimpleMath::Matrix m_View;
    DirectX::SimpleMath::Matrix m_Projection;
    DirectX::SimpleMath::Matrix m_ViewProjection;
    DirectX::SimpleMath::Matrix m_ProjectionViewInverse;

    std::unique_ptr<DirectX::SpriteFont> m_Font;
    std::unique_ptr<DirectX::SpriteBatch> m_SpriteBatch;

    ID3D11Texture2D* m_SceneTexture;
    ID3D11RenderTargetView* m_SceneView;
};
