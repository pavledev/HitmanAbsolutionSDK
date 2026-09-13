#include <directxtk/DDSTextureLoader.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Glacier/ZRender.h"
#include "Glacier/ZLevelManager.h"
#include "Glacier/ZGraphicsSettingsManager.h"
#include "Glacier/ZApplication.h"
#include "Glacier/ZScaleform.h"

#include "Renderer/DirectXRenderer.h"
#include "Renderer/ImGuiRenderer.h"
#include "Globals.h"
#include "Hooks.h"
#include "Connection/PipeServer.h"
#include "SDK.h"
#include "Fonts.h"

DirectXRenderer::DirectXRenderer()
{
    m_IsRendererSetup = false;
    m_Hwnd = nullptr;
    m_WindowWidth = 0;
    m_WindowHeight = 0;
    m_InputLayout = nullptr;
}

DirectXRenderer::~DirectXRenderer() {}

bool DirectXRenderer::Setup()
{
    if (m_IsRendererSetup)
    {
        return true;
    }

    m_Hwnd = Globals::GraphicsSettingsManager->m_hWnd;

    ZRenderDevice* renderDevice = Globals::RenderManager->m_pRenderDevice;
    ID3D11Device* device = renderDevice->m_pDirect3DDevice;
    ID3D11DeviceContext* immediateContext = renderDevice->m_pDeviceContextImmediate;
    RECT rect = { 0, 0, 0, 0 };

    GetClientRect(m_Hwnd, &rect);

    m_WindowWidth = static_cast<float>(rect.right - rect.left);
    m_WindowHeight = static_cast<float>(rect.bottom - rect.top);

    m_LineBatch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>(immediateContext);
    m_LineEffect = std::make_unique<DirectX::BasicEffect>(device);

    void const* shaderByteCode;
    size_t byteCodeLength;

    m_LineEffect->SetVertexColorEnabled(true);
    m_LineEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

    HRESULT hr = renderDevice->m_pDirect3DDevice->CreateInputLayout(
        DirectX::VertexPositionColor::InputElements, DirectX::VertexPositionColor::InputElementCount, shaderByteCode, byteCodeLength, &m_InputLayout
    );

    if (FAILED(hr))
    {
        Logger::Error("Unable to create device input layout.");

        return false;
    }

    m_Font = std::make_unique<DirectX::SpriteFont>(device, RobotoRegularSpritefontData, RobotoRegularSpritefontSize);
    m_SpriteBatch = std::make_unique<DirectX::SpriteBatch>(immediateContext);

    D3D11_VIEWPORT viewport = { 0.0f, 0.0f, m_WindowWidth, m_WindowHeight, D3D11_MIN_DEPTH, D3D11_MAX_DEPTH };

    m_SpriteBatch->SetViewport(viewport);

    m_LineEffect->SetWorld(m_World);
    m_LineEffect->SetView(m_View);
    m_LineEffect->SetProjection(m_Projection);

    CD3D11_TEXTURE2D_DESC sceneDesc(
        DXGI_FORMAT_R8G8B8A8_UNORM, m_WindowWidth, m_WindowHeight, 1, 1, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE
    );

    device->CreateTexture2D(&sceneDesc, nullptr, &m_SceneTexture);
    device->CreateRenderTargetView(m_SceneTexture, nullptr, &m_SceneView);

    Logger::Info("DirectX renderer successfully set up.");

    m_IsRendererSetup = true;

    return true;
}

void DirectXRenderer::OnPresent(ZRenderDevice* p_RenderDevice)
{
    if (!Setup())
    {
        Logger::Error("Failed to set up DirectXTK renderer.");
        Cleanup();

        return;
    }

    if (Globals::ScaleformManager->m_bIsInMainMenu || Globals::HUDManager->m_bPauseMenuActive)
    {
        return;
    }

    ID3D11Device* device = p_RenderDevice->m_pDirect3DDevice;
    ID3D11DeviceContext* immediateContext = p_RenderDevice->m_pDeviceContextImmediate;
    ZHitman5* player = Globals::LevelManager->m_rHitman.m_pInterfaceRef;

    if (player)
    {
        ZCameraEntity* activeCamera = (*Globals::ApplicationEngineWin32)->GetActiveCamera();
        const SMatrix viewMatrix = activeCamera->GetViewMatrix();
        const SMatrix projectionMatrix = activeCamera->GetProjectionMatrix();

        m_View = *reinterpret_cast<DirectX::FXMMATRIX*>(&viewMatrix);
        m_Projection = *reinterpret_cast<DirectX::FXMMATRIX*>(&projectionMatrix);

        m_ViewProjection = m_View * m_Projection;
        m_ProjectionViewInverse = (m_Projection * m_View).Invert();

        m_LineEffect->SetView(m_View);
        m_LineEffect->SetProjection(m_Projection);

        std::unique_ptr<DirectX::CommonStates> states = std::make_unique<DirectX::CommonStates>(device);

        immediateContext->OMSetBlendState(states->AlphaBlend(), nullptr, 0xFFFFFFFF);
        immediateContext->OMSetDepthStencilState(states->DepthReadReverseZ(), 0);
        immediateContext->RSSetState(states->CullNone());

        m_LineEffect->Apply(immediateContext);
        immediateContext->IASetInputLayout(m_InputLayout);

        // immediateContext->OMSetRenderTargets(1, &sceneView, nullptr);

        Render();
    }
}

void DirectXRenderer::OnResize(const SRenderDestinationDesc* p_Description)
{
    if (m_SpriteBatch.get())
    {
        m_WindowWidth = static_cast<float>(p_Description->m_nWidth);
        m_WindowHeight = static_cast<float>(p_Description->m_nHeight);

        D3D11_VIEWPORT viewport = { 0.0f, 0.0f, m_WindowWidth, m_WindowHeight, D3D11_MIN_DEPTH, D3D11_MAX_DEPTH };

        m_SpriteBatch->SetViewport(viewport);
    }
}

void DirectXRenderer::Cleanup()
{
    m_LineEffect.reset();
    m_LineBatch.reset();
    m_Font.reset();
    m_SpriteBatch.reset();
    m_InputLayout->Release();
}

void DirectXRenderer::Render()
{
    m_SpriteBatch->Begin();
    m_LineBatch->Begin();

    SDK::GetInstance().OnDraw3D();

    m_LineBatch->End();
    m_SpriteBatch->End();
}

void DirectXRenderer::DrawLine3D(const SVector3& p_From, const SVector4& p_FromColor, const SVector3& p_To, const SVector4& p_ToColor)
{
    DirectX::VertexPositionColor from2(
        DirectX::SimpleMath::Vector3(p_From.x, p_From.y, p_From.z),
        DirectX::SimpleMath::Vector4(p_FromColor.x, p_FromColor.y, p_FromColor.z, p_FromColor.w)
    );

    DirectX::VertexPositionColor to2(
        DirectX::SimpleMath::Vector3(p_To.x, p_To.y, p_To.z), DirectX::SimpleMath::Vector4(p_ToColor.x, p_ToColor.y, p_ToColor.z, p_ToColor.w)
    );

    m_LineBatch->DrawLine(from2, to2);
}

void DirectXRenderer::DrawQuad3D(
    const SVector3& p_Vertex1, const SVector4& p_Color1, const SVector3& p_Vertex2, const SVector4& p_Color2, const SVector3& p_Vertex3,
    const SVector4& p_Color3, const SVector3& p_Vertex4, const SVector4& p_Color4
)
{
    m_LineBatch->DrawQuad(
        DirectX::VertexPositionColor(
            DirectX::SimpleMath::Vector3(p_Vertex1.x, p_Vertex1.y, p_Vertex1.z),
            DirectX::SimpleMath::Vector4(p_Color1.x, p_Color1.y, p_Color1.z, p_Color1.w)
        ),
        DirectX::VertexPositionColor(
            DirectX::SimpleMath::Vector3(p_Vertex2.x, p_Vertex2.y, p_Vertex2.z),
            DirectX::SimpleMath::Vector4(p_Color2.x, p_Color2.y, p_Color2.z, p_Color2.w)
        ),
        DirectX::VertexPositionColor(
            DirectX::SimpleMath::Vector3(p_Vertex3.x, p_Vertex3.y, p_Vertex3.z),
            DirectX::SimpleMath::Vector4(p_Color3.x, p_Color3.y, p_Color3.z, p_Color3.w)
        ),
        DirectX::VertexPositionColor(
            DirectX::SimpleMath::Vector3(p_Vertex4.x, p_Vertex4.y, p_Vertex4.z),
            DirectX::SimpleMath::Vector4(p_Color4.x, p_Color4.y, p_Color4.z, p_Color4.w)
        )
    );
}

void DirectXRenderer::DrawText2D(
    const ZString& p_Text, const SVector2& p_Position, const SVector4& p_Color, float p_Rotation, float p_Scale, TextAlignment p_Alignment
)
{
    const std::string text2(p_Text.ToCString(), p_Text.Length());
    const DirectX::SimpleMath::Vector2 stringSize = m_Font->MeasureString(text2.c_str());

    DirectX::SimpleMath::Vector2 origin(0.f, 0.f);

    if (p_Alignment == TextAlignment::Center)
    {
        origin.x = stringSize.x / 2.f;
    }
    else if (p_Alignment == TextAlignment::Right)
    {
        origin.x = stringSize.x;
    }

    m_Font->DrawString(
        m_SpriteBatch.get(), text2.c_str(), DirectX::SimpleMath::Vector2(p_Position.x, p_Position.y),
        DirectX::SimpleMath::Vector4(p_Color.x, p_Color.y, p_Color.z, p_Color.w), p_Rotation, origin, p_Scale
    );
}

bool DirectXRenderer::WorldToScreen(const SVector3& p_WorldPosition, SVector2& p_Out)
{
    const DirectX::SimpleMath::Vector4 world(p_WorldPosition.x, p_WorldPosition.y, p_WorldPosition.z, 1.f);
    const DirectX::SimpleMath::Vector4 projected = DirectX::XMVector4Transform(world, m_ViewProjection);

    if (projected.w <= 0.000001f)
    {
        return false;
    }

    const float invertedZ = 1.f / projected.w;
    const DirectX::SimpleMath::Vector3 finalProjected(projected.x * invertedZ, projected.y * invertedZ, projected.z * invertedZ);

    p_Out.x = (1.f + finalProjected.x) * 0.5f * m_WindowWidth;
    p_Out.y = (1.f - finalProjected.y) * 0.5f * m_WindowHeight;

    return true;
}

bool DirectXRenderer::ScreenToWorld(const SVector2& p_ScreenPosition, SVector3& p_OutWorldPosition, SVector3& p_OutDirection)
{
    ZHitman5* player = Globals::LevelManager->m_rHitman.m_pInterfaceRef;

    if (!player)
    {
        return false;
    }

    ZCameraEntity* activeCamera = (*Globals::ApplicationEngineWin32)->GetActiveCamera();
    SMatrix cameraTrans = activeCamera->GetObjectToWorldMatrix();

    auto screenPosition =
        DirectX::SimpleMath::Vector3((2.0f * p_ScreenPosition.x) / m_WindowWidth - 1.0f, 1.0f - (2.0f * p_ScreenPosition.y) / m_WindowHeight, 1.f);
    auto rayClip = DirectX::SimpleMath::Vector4(screenPosition.x, screenPosition.y, 0.f, 1.f);

    DirectX::SimpleMath::Vector4 rayEye = DirectX::XMVector4Transform(rayClip, m_Projection.Invert());
    rayEye.z = -1.f;
    rayEye.w = 0.f;

    DirectX::SimpleMath::Vector4 rayWorld = DirectX::XMVector4Transform(rayEye, m_View.Invert());
    rayWorld.Normalize();

    p_OutWorldPosition.x = cameraTrans.Trans.x + rayWorld.x;
    p_OutWorldPosition.y = cameraTrans.Trans.y + rayWorld.y;
    p_OutWorldPosition.z = cameraTrans.Trans.z + rayWorld.z;

    p_OutDirection.x = rayWorld.x;
    p_OutDirection.y = rayWorld.y;
    p_OutDirection.z = rayWorld.z;

    return true;
}

void DirectXRenderer::DrawBox3D(const SVector3& p_Min, const SVector3& p_Max, const SVector4& p_Color)
{
    SVector3 corners[] = {
        SVector3(p_Min.x, p_Min.y, p_Min.z), SVector3(p_Min.x, p_Max.y, p_Min.z), SVector3(p_Max.x, p_Max.y, p_Min.z),
        SVector3(p_Max.x, p_Min.y, p_Min.z), SVector3(p_Max.x, p_Max.y, p_Max.z), SVector3(p_Min.x, p_Max.y, p_Max.z),
        SVector3(p_Min.x, p_Min.y, p_Max.z), SVector3(p_Max.x, p_Min.y, p_Max.z),
    };

    DrawLine3D(corners[0], p_Color, corners[1], p_Color);
    DrawLine3D(corners[1], p_Color, corners[2], p_Color);
    DrawLine3D(corners[2], p_Color, corners[3], p_Color);
    DrawLine3D(corners[3], p_Color, corners[0], p_Color);

    DrawLine3D(corners[4], p_Color, corners[5], p_Color);
    DrawLine3D(corners[5], p_Color, corners[6], p_Color);
    DrawLine3D(corners[6], p_Color, corners[7], p_Color);
    DrawLine3D(corners[7], p_Color, corners[4], p_Color);

    DrawLine3D(corners[1], p_Color, corners[5], p_Color);
    DrawLine3D(corners[0], p_Color, corners[6], p_Color);

    DrawLine3D(corners[2], p_Color, corners[4], p_Color);
    DrawLine3D(corners[3], p_Color, corners[7], p_Color);
}

SVector3 XMVecToSVec3(const DirectX::XMVECTOR& vector)
{
    return SVector3(DirectX::XMVectorGetX(vector), DirectX::XMVectorGetY(vector), DirectX::XMVectorGetZ(vector));
}

void DirectXRenderer::DrawOBB3D(const SVector3& p_Min, const SVector3& p_Max, const SMatrix& p_Transform, const SVector4& p_Color)
{
    const auto transform2 = *reinterpret_cast<DirectX::FXMMATRIX*>(&p_Transform);

    DirectX::XMVECTOR corners[] = {
        DirectX::XMVector3Transform(DirectX::SimpleMath::Vector3(p_Min.x, p_Min.y, p_Min.z), transform2),
        DirectX::XMVector3Transform(DirectX::SimpleMath::Vector3(p_Min.x, p_Max.y, p_Min.z), transform2),
        DirectX::XMVector3Transform(DirectX::SimpleMath::Vector3(p_Max.x, p_Max.y, p_Min.z), transform2),
        DirectX::XMVector3Transform(DirectX::SimpleMath::Vector3(p_Max.x, p_Min.y, p_Min.z), transform2),
        DirectX::XMVector3Transform(DirectX::SimpleMath::Vector3(p_Max.x, p_Max.y, p_Max.z), transform2),
        DirectX::XMVector3Transform(DirectX::SimpleMath::Vector3(p_Min.x, p_Max.y, p_Max.z), transform2),
        DirectX::XMVector3Transform(DirectX::SimpleMath::Vector3(p_Min.x, p_Min.y, p_Max.z), transform2),
        DirectX::XMVector3Transform(DirectX::SimpleMath::Vector3(p_Max.x, p_Min.y, p_Max.z), transform2),
    };

    DrawLine3D(XMVecToSVec3(corners[0]), p_Color, XMVecToSVec3(corners[1]), p_Color);
    DrawLine3D(XMVecToSVec3(corners[1]), p_Color, XMVecToSVec3(corners[2]), p_Color);
    DrawLine3D(XMVecToSVec3(corners[2]), p_Color, XMVecToSVec3(corners[3]), p_Color);
    DrawLine3D(XMVecToSVec3(corners[3]), p_Color, XMVecToSVec3(corners[0]), p_Color);

    DrawLine3D(XMVecToSVec3(corners[4]), p_Color, XMVecToSVec3(corners[5]), p_Color);
    DrawLine3D(XMVecToSVec3(corners[5]), p_Color, XMVecToSVec3(corners[6]), p_Color);
    DrawLine3D(XMVecToSVec3(corners[6]), p_Color, XMVecToSVec3(corners[7]), p_Color);
    DrawLine3D(XMVecToSVec3(corners[7]), p_Color, XMVecToSVec3(corners[4]), p_Color);

    DrawLine3D(XMVecToSVec3(corners[1]), p_Color, XMVecToSVec3(corners[5]), p_Color);
    DrawLine3D(XMVecToSVec3(corners[0]), p_Color, XMVecToSVec3(corners[6]), p_Color);

    DrawLine3D(XMVecToSVec3(corners[2]), p_Color, XMVecToSVec3(corners[4]), p_Color);
    DrawLine3D(XMVecToSVec3(corners[3]), p_Color, XMVecToSVec3(corners[7]), p_Color);
}

void DirectXRenderer::CreateDDSTextureFromMemory(
    const void* data, const unsigned int resourceDataSize, ID3D11Resource** texture, ID3D11ShaderResourceView** textureView, float& width,
    float& height
)
{
    ID3D11Device* device = Globals::RenderManager->m_pRenderDevice->m_pDirect3DDevice;
    HRESULT result = DirectX::CreateDDSTextureFromMemory(device, static_cast<const unsigned char*>(data), resourceDataSize, texture, textureView);

    if (FAILED(result))
    {
        Logger::Error("Failed to create DDS texture!");

        return;
    }

    ID3D11Texture2D* texture2D = nullptr;
    result = (*texture)->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&texture2D));

    if (FAILED(result))
    {
        Logger::Error("Failed to query ID3D11Texture2D interface!");

        return;
    }

    if (texture2D)
    {
        D3D11_TEXTURE2D_DESC desc;

        texture2D->GetDesc(&desc);

        width = static_cast<float>(desc.Width);
        height = static_cast<float>(desc.Height);

        texture2D->Release();
    }
}

void DirectXRenderer::LoadTextureFromFile(
    const char* textureFilePath, ID3D11Texture2D** texture, ID3D11ShaderResourceView** texureView, unsigned int& width, unsigned int& height
)
{
    ID3D11Device* device = Globals::RenderManager->m_pRenderDevice->m_pDirect3DDevice;
    int imageWidth = 0;
    int imageHeight = 0;
    unsigned char* imageData = stbi_load(textureFilePath, &imageWidth, &imageHeight, nullptr, 4);

    if (!imageData)
    {
        Logger::Error("Failed to load texture!");

        return;
    }

    width = static_cast<unsigned int>(imageWidth);
    height = static_cast<unsigned int>(imageHeight);

    D3D11_TEXTURE2D_DESC desc;

    ZeroMemory(&desc, sizeof(desc));

    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA subResource;
    subResource.pSysMem = imageData;
    subResource.SysMemPitch = desc.Width * 4;
    subResource.SysMemSlicePitch = 0;

    HRESULT result = device->CreateTexture2D(&desc, &subResource, texture);

    if (FAILED(result))
    {
        Logger::Error("Failed to create texture!");

        stbi_image_free(imageData);

        return;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

    ZeroMemory(&srvDesc, sizeof(srvDesc));

    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = desc.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;

    result = device->CreateShaderResourceView(*texture, &srvDesc, texureView);

    if (FAILED(result))
    {
        Logger::Error("Failed to create DDS texture!");

        stbi_image_free(imageData);

        return;
    }

    stbi_image_free(imageData);
}
