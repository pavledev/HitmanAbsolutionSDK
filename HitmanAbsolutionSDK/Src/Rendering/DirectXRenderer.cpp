#include <d3dcompiler.h>

#include "Glacier/ZRender.h"
#include "Glacier/ZLevelManager.h"
#include "Glacier/ZGraphicsSettingsManager.h"
#include "Glacier/ZApplication.h"
#include "Glacier/ZScaleform.h"
#include "Glacier/MDF_FONT.h"

#include "DirectXRenderer.h"
#include "ModSDK.h"
#include "Fonts.h"
#include "Utils/D3DUtils.h"
#include "D3D11StateGuard.h"

DirectXRenderer::~DirectXRenderer() {}

bool DirectXRenderer::SetupRenderer()
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

    m_PrimitiveBatch = std::make_unique<CustomPrimitiveBatch<DirectX::VertexPositionColor>>(
        immediateContext,
        [this, immediateContext]()
        {
            m_PrimitiveEffect->Apply(immediateContext);
            immediateContext->IASetInputLayout(m_PrimitiveInputLayout.m_Ref);
        }
    );
    m_PrimitiveEffect = std::make_unique<DirectX::BasicEffect>(device);

    void const* shaderByteCode;
    size_t byteCodeLength;

    m_PrimitiveEffect->SetVertexColorEnabled(true);
    m_PrimitiveEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

    HRESULT result = renderDevice->m_pDirect3DDevice->CreateInputLayout(
        DirectX::VertexPositionColor::InputElements, DirectX::VertexPositionColor::InputElementCount, shaderByteCode, byteCodeLength,
        &m_PrimitiveInputLayout.m_Ref
    );

    if (FAILED(result))
    {
        Logger::Error("Unable to create primitive input layout.");
        return false;
    }

    m_TextBatch = std::make_unique<CustomPrimitiveBatch<DirectX::VertexPositionColorTexture>>(
        renderDevice->m_pDeviceContextImmediate,
        [this, immediateContext]()
        {
            m_TextEffect->Apply(immediateContext);
            immediateContext->IASetInputLayout(m_TextInputLayout.m_Ref);

            immediateContext->VSSetShader(m_FontVertexShader.m_Ref, nullptr, 0);
            immediateContext->PSSetShader(m_FontPixelShader.m_Ref, nullptr, 0);
            immediateContext->PSSetSamplers(0, 1, &m_LinearClamp.m_Ref);

            immediateContext->OMSetDepthStencilState(m_DepthRead, 0);
        }
    );
    m_TextEffect = std::make_unique<DirectX::BasicEffect>(device);

    m_TextEffect->SetVertexColorEnabled(true);
    m_TextEffect->SetTextureEnabled(true);
    m_TextEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

    if (!CreateFontDistanceFieldTexture())
    {
        return false;
    }

    MDF_FONT::Initialize();

    m_TextEffect->SetTexture(m_FontDistanceFieldSRV.m_Ref);

    {
        D3D11_RASTERIZER_DESC rasterizerDesc = {};
        rasterizerDesc.FillMode = D3D11_FILL_SOLID;
        rasterizerDesc.CullMode = D3D11_CULL_NONE;
        rasterizerDesc.FrontCounterClockwise = TRUE;
        rasterizerDesc.DepthBias = 0;
        rasterizerDesc.DepthBiasClamp = 0.0f;
        rasterizerDesc.SlopeScaledDepthBias = -2.0f;
        rasterizerDesc.DepthClipEnable = FALSE;
        rasterizerDesc.ScissorEnable = FALSE;
        rasterizerDesc.MultisampleEnable = FALSE;
        rasterizerDesc.AntialiasedLineEnable = TRUE;

        result = device->CreateRasterizerState(&rasterizerDesc, &m_CullNone.m_Ref);

        if (FAILED(result))
        {
            Logger::Error("Unable to create rasterizer state!");
            return false;
        }

        rasterizerDesc.CullMode = D3D11_CULL_BACK;

        result = device->CreateRasterizerState(&rasterizerDesc, &m_CullBack.m_Ref);

        if (FAILED(result))
        {
            Logger::Error("Unable to create rasterizer state!");
            return false;
        }

        D3D11_BLEND_DESC blendDesc = {};
        blendDesc.AlphaToCoverageEnable = FALSE;
        blendDesc.IndependentBlendEnable = TRUE;

        D3D11_RENDER_TARGET_BLEND_DESC rtBlendDesc = {};

        rtBlendDesc.BlendEnable = TRUE;
        rtBlendDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
        rtBlendDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        rtBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;
        rtBlendDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
        rtBlendDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
        rtBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
        rtBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_RED | D3D11_COLOR_WRITE_ENABLE_GREEN | D3D11_COLOR_WRITE_ENABLE_BLUE;

        blendDesc.RenderTarget[0] = rtBlendDesc;

        result = device->CreateBlendState(&blendDesc, &m_AlphaBlend.m_Ref);

        if (FAILED(result))
        {
            Logger::Error("Unable to create blend state!");
            return false;
        }

        D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
        ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

        depthStencilDesc.DepthEnable = TRUE;
        depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

        depthStencilDesc.StencilEnable = FALSE;
        depthStencilDesc.StencilReadMask = 0xFF;
        depthStencilDesc.StencilWriteMask = 0xFF;

        D3D11_DEPTH_STENCILOP_DESC stencilOpDesc = {};

        stencilOpDesc.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        stencilOpDesc.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
        stencilOpDesc.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        stencilOpDesc.StencilFunc = D3D11_COMPARISON_ALWAYS;

        depthStencilDesc.FrontFace = stencilOpDesc;
        depthStencilDesc.BackFace = stencilOpDesc;

        result = device->CreateDepthStencilState(&depthStencilDesc, &m_DepthDefault.m_Ref);

        if (FAILED(result))
        {
            Logger::Error("Unable to create depth stencil state!");
            return false;
        }

        depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

        result = device->CreateDepthStencilState(&depthStencilDesc, &m_DepthRead.m_Ref);

        if (FAILED(result))
        {
            Logger::Error("Unable to create depth stencil state!");
            return false;
        }

        D3D11_SAMPLER_DESC samplerDesc = {};
        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.MipLODBias = 0.f;
        samplerDesc.MaxAnisotropy = 1;
        samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        samplerDesc.BorderColor[0] = 0.f;
        samplerDesc.BorderColor[1] = 0.f;
        samplerDesc.BorderColor[2] = 0.f;
        samplerDesc.BorderColor[3] = 0.f;
        samplerDesc.MinLOD = 0.f;
        samplerDesc.MaxLOD = 16.f;

        result = device->CreateSamplerState(&samplerDesc, &m_LinearClamp.m_Ref);

        if (FAILED(result))
        {
            Logger::Error("Unable to create sampler state!");
            return false;
        }

        const std::string debugRenderDistanceFieldFontVertexShader = R"(
			cbuffer Parameters : register(b0)
			{
				float4 DiffuseColor             : packoffset(c0);
				float3 EmissiveColor            : packoffset(c1);
				float3 SpecularColor            : packoffset(c2);
				float  SpecularPower            : packoffset(c2.w);

				float3 LightDirection[3]        : packoffset(c3);
				float3 LightDiffuseColor[3]     : packoffset(c6);
				float3 LightSpecularColor[3]    : packoffset(c9);

				float3 EyePosition              : packoffset(c12);

				float3 FogColor                 : packoffset(c13);
				float4 FogVector                : packoffset(c14);

				float4x4 World                  : packoffset(c15);
				float3x3 WorldInverseTranspose  : packoffset(c19);
				float4x4 WorldViewProj          : packoffset(c22);
			};

			struct VSInputTxVc
			{
				float4 Position : SV_Position;
				float2 TexCoord : TEXCOORD0;
				float4 Color    : COLOR;
			};

			struct VSOutputTxNoFog
			{
				float4 Diffuse    : COLOR0;
				float2 TexCoord   : TEXCOORD0;
				float4 PositionPS : SV_Position;
			};

			struct CommonVSOutput
			{
				float4 Pos_ps;
				float4 Diffuse;
				float3 Specular;
				float  FogFactor;
			};

			float ComputeFogFactor(float4 position)
			{
				return saturate(dot(position, FogVector));
			}

			CommonVSOutput ComputeCommonVSOutput(float4 position)
			{
				CommonVSOutput vout;

				vout.Pos_ps = mul(position, WorldViewProj);
				vout.Diffuse = DiffuseColor;
				vout.Specular = 0;
				vout.FogFactor = ComputeFogFactor(position);

				return vout;
			}

			#define SetCommonVSOutputParamsNoFog \
				vout.PositionPS = cout.Pos_ps; \
				vout.Diffuse = cout.Diffuse;

		    VSOutputTxNoFog VSBasicTxVcNoFog(VSInputTxVc vin)
			{
				VSOutputTxNoFog vout;

				CommonVSOutput cout = ComputeCommonVSOutput(vin.Position);
				SetCommonVSOutputParamsNoFog;

				vout.TexCoord = vin.TexCoord;
				vout.Diffuse *= vin.Color;

				return vout;
			}
		)";

        const std::string debugRenderDistanceFieldFontPixelShader = R"(
		    Texture2D<float4> mapDebug2DLinear : register(t0);
			sampler samplerLinearClamp : register(s0);

			struct PSInput
			{
				float4 color : COLOR0;
				float2 uv : TEXCOORD0;
			};

			float4 mainPS(PSInput input) : SV_Target0
			{
				// Sample the font distance field texture
				float distance = mapDebug2DLinear.Sample(samplerLinearClamp, input.uv).x;

				// Shift and scale the distance to control edge sharpness
				const float edgeThreshold = -0.4f; // Shift distance for smoothing
				const float scaleFactor = 5.0f; // Scale to sharpen the edge
				distance = saturate((distance + edgeThreshold) * scaleFactor);

				// Further control the smoothness of the transition
				float smoothedAlpha = distance * distance * (-2.0f) + 3.0f * distance;

				// Output the final color with computed alpha
				float alpha = smoothedAlpha * distance;
    
				return float4(input.color.xyz, alpha);
			}
		)";

        ScopedD3DRef<ID3DBlob> fontVertexShaderBlob;
        ScopedD3DRef<ID3DBlob> fontPixelShaderBlob;

        if (!CompileShaderFromString(debugRenderDistanceFieldFontVertexShader, "VSBasicTxVcNoFog", "vs_5_0", &fontVertexShaderBlob.m_Ref))
        {
            return false;
        }

        if (!CompileShaderFromString(debugRenderDistanceFieldFontPixelShader, "mainPS", "ps_5_0", &fontPixelShaderBlob.m_Ref))
        {
            return false;
        }

        result = renderDevice->m_pDirect3DDevice->CreateInputLayout(
            DirectX::VertexPositionColorTexture::InputElements, DirectX::VertexPositionColorTexture::InputElementCount,
            fontVertexShaderBlob->GetBufferPointer(), fontVertexShaderBlob->GetBufferSize(), &m_TextInputLayout.m_Ref
        );

        if (FAILED(result))
        {
            Logger::Error("Unable to create text input layout.");
            return false;
        }

        result = device->CreateVertexShader(
            fontVertexShaderBlob->GetBufferPointer(), fontVertexShaderBlob->GetBufferSize(), nullptr, &m_FontVertexShader.m_Ref
        );

        if (FAILED(result))
        {
            Logger::Error("Unable to create font vertex shader!");
            return false;
        }

        result = device->CreatePixelShader(
            fontPixelShaderBlob->GetBufferPointer(), fontPixelShaderBlob->GetBufferSize(), nullptr, &m_FontPixelShader.m_Ref
        );

        if (FAILED(result))
        {
            Logger::Error("Unable to create font pixel shader!");
            return false;
        }

        const D3D11_INPUT_ELEMENT_DESC inputElementDescs[] = { { "POSITION", 0, DXGI_FORMAT_R16G16B16A16_SNORM, 0, 0, D3D11_INPUT_PER_VERTEX_DATA,
                                                                 0 },
                                                               { "NORMAL", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
                                                               { "TANGENT", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 1, 4, D3D11_INPUT_PER_VERTEX_DATA, 0 },
                                                               { "BINORMAL", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 1, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
                                                               { "TEXCOORD", 0, DXGI_FORMAT_R16G16_SNORM, 1, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
                                                               { "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 } };

        m_DebugEffect = std::make_unique<DebugEffect>(device, inputElementDescs, _countof(inputElementDescs));
    }

    m_Font = std::make_unique<DirectX::SpriteFont>(device, RobotoRegularSpritefontData, RobotoRegularSpritefontSize);
    m_SpriteBatch = std::make_unique<DirectX::SpriteBatch>(immediateContext);

    D3D11_VIEWPORT viewport = { 0.0f, 0.0f, m_WindowWidth, m_WindowHeight, D3D11_MIN_DEPTH, D3D11_MAX_DEPTH };

    m_SpriteBatch->SetViewport(viewport);

    m_PrimitiveEffect->SetWorld(m_World);
    m_PrimitiveEffect->SetView(m_View);
    m_PrimitiveEffect->SetProjection(m_Projection);

    m_TextEffect->SetWorld(m_World);
    m_TextEffect->SetView(m_View);
    m_TextEffect->SetProjection(m_Projection);

    m_DebugEffect->SetWorld(m_World);
    m_DebugEffect->SetView(m_View);
    m_DebugEffect->SetProjection(m_Projection);

    Logger::Info("DirectX renderer successfully set up.");

    m_IsRendererSetup = true;

    return true;
}

void DirectXRenderer::TeardownRenderer()
{
    m_IsRendererSetup = false;

    m_PrimitiveBatch.reset();
    m_TextBatch.reset();

    m_SpriteBatch.reset();
    m_Font.reset();

    m_DebugEffect.reset();
    m_TextEffect.reset();
    m_PrimitiveEffect.reset();

    m_PrimitiveInputLayout.Reset();
    m_TextInputLayout.Reset();

    m_FontVertexShader.Reset();
    m_FontPixelShader.Reset();

    m_FontDistanceFieldSRV.Reset();
    m_FontDistanceFieldTexture.Reset();

    m_LinearClamp.Reset();

    m_DepthRead.Reset();
    m_DepthDefault.Reset();

    m_AlphaBlend.Reset();

    m_CullBack.Reset();
    m_CullNone.Reset();
}

void DirectXRenderer::Draw(ZRenderTargetView* p_RenderTargetView)
{
    ID3D11DeviceContext* immediateContext = Globals::RenderManager->m_pRenderDevice->m_pDeviceContextImmediate;

    D3D11StateGuard stateGuard(immediateContext);

    immediateContext->OMSetBlendState(m_AlphaBlend.m_Ref, nullptr, 0xFFFFFFFF);
    immediateContext->OMSetDepthStencilState(nullptr, 0);
    immediateContext->RSSetState(m_CullNone.m_Ref);
    immediateContext->OMSetRenderTargets(1, &p_RenderTargetView->m_pRTV, nullptr);

    m_PrimitiveBatch->Begin();
    m_TextBatch->Begin();

    ModSDK::GetInstance().OnDraw3D();

    m_PrimitiveBatch->End();
    m_TextBatch->End();

    m_SpriteBatch->Begin();

    ModSDK::GetInstance().OnDraw2D();

    m_SpriteBatch->End();
}

void DirectXRenderer::DepthDraw(ZRenderTargetView* p_RenderTargetView, ZRenderDepthStencilView* p_RenderDepthStencilView)
{
    ID3D11DeviceContext* immediateContext = Globals::RenderManager->m_pRenderDevice->m_pDeviceContextImmediate;

    D3D11StateGuard stateGuard(immediateContext);

    immediateContext->OMSetBlendState(m_AlphaBlend.m_Ref, nullptr, 0xFFFFFFFF);
    immediateContext->OMSetDepthStencilState(m_DepthDefault.m_Ref, 0);
    immediateContext->RSSetState(m_CullNone.m_Ref);
    immediateContext->OMSetRenderTargets(1, &p_RenderTargetView->m_pRTV, p_RenderDepthStencilView->m_pDSV);

    m_PrimitiveBatch->Begin();
    m_TextBatch->Begin();

    ModSDK::GetInstance().OnDepthDraw3D();

    m_PrimitiveBatch->End();
    m_TextBatch->End();
}

void DirectXRenderer::OnEngineInitialized()
{
    Hooks::ZRenderGraphNodeRenderer_DrawOSDAndScaleform->AddDetour(this, &DirectXRenderer::ZRenderGraphNodeRenderer_DrawOSDAndScaleform);
}

void DirectXRenderer::OnPresent(ZRenderDevice* p_RenderDevice)
{
    if (!SetupRenderer())
    {
        Logger::Error("Failed to set up DirectXTK renderer.");
        TeardownRenderer();

        return;
    }
}

void DirectXRenderer::OnResize(const SRenderDestinationDesc* p_Description)
{
    if (!m_SpriteBatch.get())
    {
        return;
    }

    m_WindowWidth = static_cast<float>(p_Description->m_nWidth);
    m_WindowHeight = static_cast<float>(p_Description->m_nHeight);

    D3D11_VIEWPORT viewport = { 0.0f, 0.0f, m_WindowWidth, m_WindowHeight, D3D11_MIN_DEPTH, D3D11_MAX_DEPTH };

    m_SpriteBatch->SetViewport(viewport);
}

bool DirectXRenderer::WorldToScreen(const SVector3& p_WorldPos, SVector2& p_Out)
{
    if (!m_IsRendererSetup)
    {
        return false;
    }

    const DirectX::SimpleMath::Vector4 world(p_WorldPos.x, p_WorldPos.y, p_WorldPos.z, 1.f);
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

bool DirectXRenderer::ScreenToWorld(const SVector2& p_ScreenPos, SVector3& p_OutWorldPos, SVector3& p_OutDirection)
{
    if (!m_IsRendererSetup)
    {
        return false;
    }

    ZCameraEntity* activeCamera = (*Globals::ApplicationEngineWin32)->GetActiveCamera();

    if (!activeCamera)
    {
        return false;
    }

    auto cameraTransform = activeCamera->GetObjectToWorldMatrix();

    auto screenPosition =
        DirectX::SimpleMath::Vector3((2.0f * p_ScreenPos.x) / m_WindowWidth - 1.0f, 1.0f - (2.0f * p_ScreenPos.y) / m_WindowHeight, 1.f);
    auto rayClip = DirectX::SimpleMath::Vector4(screenPosition.x, screenPosition.y, 0.f, 1.f);

    DirectX::SimpleMath::Vector4 rayEye = DirectX::XMVector4Transform(rayClip, m_Projection.Invert());
    rayEye.z = -1.f;
    rayEye.w = 0.f;

    DirectX::SimpleMath::Vector4 rayWorld = DirectX::XMVector4Transform(rayEye, m_View.Invert());
    rayWorld.Normalize();

    p_OutWorldPos.x = cameraTransform.Trans.x + rayWorld.x;
    p_OutWorldPos.y = cameraTransform.Trans.y + rayWorld.y;
    p_OutWorldPos.z = cameraTransform.Trans.z + rayWorld.z;

    p_OutDirection.x = rayWorld.x;
    p_OutDirection.y = rayWorld.y;
    p_OutDirection.z = rayWorld.z;

    return true;
}

void DirectXRenderer::DrawLine3D(const SVector3& p_From, const SVector4& p_FromColor, const SVector3& p_To, const SVector4& p_ToColor)
{
    if (m_IsFrustumCullingEnabled && !IsPointInsideViewFrustum(p_From) && !IsPointInsideViewFrustum(p_To))
    {
        return;
    }

    DirectX::VertexPositionColor from(
        DirectX::SimpleMath::Vector3(p_From.x, p_From.y, p_From.z),
        DirectX::SimpleMath::Vector4(p_FromColor.x, p_FromColor.y, p_FromColor.z, p_FromColor.w)
    );

    DirectX::VertexPositionColor to(
        DirectX::SimpleMath::Vector3(p_To.x, p_To.y, p_To.z), DirectX::SimpleMath::Vector4(p_ToColor.x, p_ToColor.y, p_ToColor.z, p_ToColor.w)
    );

    m_PrimitiveBatch->DrawLine(from, to);
}

void DirectXRenderer::DrawBox3D(const SVector3& p_Min, const SVector3& p_Max, const SVector4& p_Color)
{
    if (m_IsFrustumCullingEnabled && !IsAABBInsideViewFrustum(p_Min, p_Max, SMatrix()))
    {
        return;
    }

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

void DirectXRenderer::DrawBox3D(const SVector3& p_Center, const SVector3& p_Size, const SMatrix& p_Transform, const SVector4& p_Color)
{
    const SVector3 halfSize = p_Size * 0.5f;

    if (m_IsFrustumCullingEnabled && !IsOBBInsideViewFrustum(p_Center, halfSize, p_Transform))
    {
        return;
    }

    const SVector3 right = p_Transform.Right * halfSize.x;
    const SVector3 forward = -(p_Transform.Backward * halfSize.y);
    const SVector3 up = p_Transform.ZAxis * halfSize.z;

    const SVector3 corners[8] = {
        p_Center - right - up - forward, p_Center - right + up - forward, p_Center + right + up - forward, p_Center + right - up - forward,
        p_Center - right - up + forward, p_Center - right + up + forward, p_Center + right + up + forward, p_Center + right - up + forward,
    };

    static constexpr int faces[12][3] = {
        { 0, 1, 2 }, { 0, 2, 3 }, // bottom
        { 4, 6, 5 }, { 4, 7, 6 }, // top
        { 0, 3, 7 }, { 0, 7, 4 }, // front
        { 1, 5, 6 }, { 1, 6, 2 }, // back
        { 0, 4, 5 }, { 0, 5, 1 }, // left
        { 3, 2, 6 }, { 3, 6, 7 }  // right
    };

    const bool wasFrustumCullingEnabled = m_IsFrustumCullingEnabled;
    m_IsFrustumCullingEnabled = false;

    for (const auto& face : faces)
    {
        DrawTriangle3D(corners[face[0]], p_Color, corners[face[1]], p_Color, corners[face[2]], p_Color);
    }

    m_IsFrustumCullingEnabled = wasFrustumCullingEnabled;
}

void DirectXRenderer::DrawBoxWire3D(const SVector3& p_Center, const SVector3& p_Size, const SMatrix& p_Transform, const SVector4& p_Color)
{
    const SVector3 halfSize = p_Size * 0.5f;

    if (m_IsFrustumCullingEnabled && !IsOBBInsideViewFrustum(p_Center, halfSize, p_Transform))
    {
        return;
    }

    const SVector3 right = p_Transform.Right * halfSize.x;
    const SVector3 forward = -(p_Transform.Backward * halfSize.y);
    const SVector3 up = p_Transform.ZAxis * halfSize.z;

    const SVector3 corners[8] = {
        p_Center - right - up - forward, p_Center - right + up - forward, p_Center + right + up - forward, p_Center + right - up - forward,
        p_Center + right + up + forward, p_Center - right + up + forward, p_Center - right - up + forward, p_Center + right - up + forward,
    };

    static constexpr int edges[12][2] = {
        { 0, 1 }, { 1, 2 }, { 2, 3 }, { 3, 0 }, // bottom face
        { 4, 5 }, { 5, 6 }, { 6, 7 }, { 7, 4 }, // top face
        { 0, 6 }, { 1, 5 }, { 2, 4 }, { 3, 7 }  // vertical edges
    };

    const bool wasFrustumCullingEnabled = m_IsFrustumCullingEnabled;
    m_IsFrustumCullingEnabled = false;

    for (const auto& edge : edges)
    {
        DrawLine3D(corners[edge[0]], p_Color, corners[edge[1]], p_Color);
    }

    m_IsFrustumCullingEnabled = wasFrustumCullingEnabled;
}

void DirectXRenderer::DrawOBB3D(const SVector3& p_Min, const SVector3& p_Max, const SMatrix& p_Transform, const SVector4& p_Color)
{
    if (m_IsFrustumCullingEnabled && !IsAABBInsideViewFrustum(p_Min, p_Max, p_Transform))
    {
        return;
    }

    const auto transform = *reinterpret_cast<DirectX::FXMMATRIX*>(&p_Transform);

    DirectX::XMVECTOR corners[] = {
        DirectX::XMVector3Transform(DirectX::SimpleMath::Vector3(p_Min.x, p_Min.y, p_Min.z), transform),
        DirectX::XMVector3Transform(DirectX::SimpleMath::Vector3(p_Min.x, p_Max.y, p_Min.z), transform),
        DirectX::XMVector3Transform(DirectX::SimpleMath::Vector3(p_Max.x, p_Max.y, p_Min.z), transform),
        DirectX::XMVector3Transform(DirectX::SimpleMath::Vector3(p_Max.x, p_Min.y, p_Min.z), transform),
        DirectX::XMVector3Transform(DirectX::SimpleMath::Vector3(p_Max.x, p_Max.y, p_Max.z), transform),
        DirectX::XMVector3Transform(DirectX::SimpleMath::Vector3(p_Min.x, p_Max.y, p_Max.z), transform),
        DirectX::XMVector3Transform(DirectX::SimpleMath::Vector3(p_Min.x, p_Min.y, p_Max.z), transform),
        DirectX::XMVector3Transform(DirectX::SimpleMath::Vector3(p_Max.x, p_Min.y, p_Max.z), transform),
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

void DirectXRenderer::DrawBoundingQuads3D(const SVector3& p_Min, const SVector3& p_Max, const SMatrix& p_Transform, const SVector4& p_Color)
{
    if (m_IsFrustumCullingEnabled && !IsAABBInsideViewFrustum(p_Min, p_Max, p_Transform))
    {
        return;
    }

    const auto transform = *reinterpret_cast<DirectX::FXMMATRIX*>(&p_Transform);

    DirectX::XMVECTOR corners[] = {
        DirectX::XMVector3Transform(DirectX::SimpleMath::Vector3(p_Min.x, p_Min.y, p_Min.z), transform),
        DirectX::XMVector3Transform(DirectX::SimpleMath::Vector3(p_Min.x, p_Max.y, p_Min.z), transform),
        DirectX::XMVector3Transform(DirectX::SimpleMath::Vector3(p_Max.x, p_Max.y, p_Min.z), transform),
        DirectX::XMVector3Transform(DirectX::SimpleMath::Vector3(p_Max.x, p_Min.y, p_Min.z), transform),
        DirectX::XMVector3Transform(DirectX::SimpleMath::Vector3(p_Max.x, p_Max.y, p_Max.z), transform),
        DirectX::XMVector3Transform(DirectX::SimpleMath::Vector3(p_Min.x, p_Max.y, p_Max.z), transform),
        DirectX::XMVector3Transform(DirectX::SimpleMath::Vector3(p_Min.x, p_Min.y, p_Max.z), transform),
        DirectX::XMVector3Transform(DirectX::SimpleMath::Vector3(p_Max.x, p_Min.y, p_Max.z), transform),
    };

    auto drawQuad = [&](int i0, int i1, int i2, int i3)
    {
        SVector3 v0(DirectX::XMVectorGetX(corners[i0]), DirectX::XMVectorGetY(corners[i0]), DirectX::XMVectorGetZ(corners[i0]));
        SVector3 v1(DirectX::XMVectorGetX(corners[i1]), DirectX::XMVectorGetY(corners[i1]), DirectX::XMVectorGetZ(corners[i1]));
        SVector3 v2(DirectX::XMVectorGetX(corners[i2]), DirectX::XMVectorGetY(corners[i2]), DirectX::XMVectorGetZ(corners[i2]));
        SVector3 v3(DirectX::XMVectorGetX(corners[i3]), DirectX::XMVectorGetY(corners[i3]), DirectX::XMVectorGetZ(corners[i3]));

        DrawTriangle3D(v0, p_Color, v1, p_Color, v2, p_Color);
        DrawTriangle3D(v0, p_Color, v2, p_Color, v3, p_Color);
    };

    drawQuad(0, 1, 2, 3); // Front face
    drawQuad(4, 5, 6, 7); // Back face
    drawQuad(0, 1, 5, 6); // Left face
    drawQuad(2, 3, 7, 4); // Right face
    drawQuad(1, 2, 4, 5); // Top face
    drawQuad(0, 3, 7, 6); // Bottom face
}

void DirectXRenderer::DrawTriangle3D(
    const SVector3& p_V1, const SVector4& p_Color1, const SVector3& p_V2, const SVector4& p_Color2, const SVector3& p_V3, const SVector4& p_Color3
)
{
    if (m_IsFrustumCullingEnabled && !IsPointInsideViewFrustum(p_V1) && !IsPointInsideViewFrustum(p_V2) && !IsPointInsideViewFrustum(p_V3))
    {
        return;
    }

    m_PrimitiveBatch->DrawTriangle(
        DirectX::VertexPositionColor(
            DirectX::SimpleMath::Vector3(p_V1.x, p_V1.y, p_V1.z), DirectX::SimpleMath::Vector4(p_Color1.x, p_Color1.y, p_Color1.z, p_Color1.w)
        ),
        DirectX::VertexPositionColor(
            DirectX::SimpleMath::Vector3(p_V2.x, p_V2.y, p_V2.z), DirectX::SimpleMath::Vector4(p_Color2.x, p_Color2.y, p_Color2.z, p_Color2.w)
        ),
        DirectX::VertexPositionColor(
            DirectX::SimpleMath::Vector3(p_V3.x, p_V3.y, p_V3.z), DirectX::SimpleMath::Vector4(p_Color3.x, p_Color3.y, p_Color3.z, p_Color3.w)
        )
    );
}

void DirectXRenderer::DrawTriangle3D(
    const SVector3& p_V1, const SVector4& p_Color1, const SVector2& p_TextureCoordinates1, const SVector3& p_V2, const SVector4& p_Color2,
    const SVector2& p_TextureCoordinates2, const SVector3& p_V3, const SVector4& p_Color3, const SVector2& p_TextureCoordinates3
)
{
    if (m_IsFrustumCullingEnabled && !IsPointInsideViewFrustum(p_V1) && !IsPointInsideViewFrustum(p_V2) && !IsPointInsideViewFrustum(p_V3))
    {
        return;
    }

    m_TextBatch->DrawTriangle(
        DirectX::VertexPositionColorTexture(
            DirectX::SimpleMath::Vector3(p_V1.x, p_V1.y, p_V1.z), DirectX::SimpleMath::Vector4(p_Color1.x, p_Color1.y, p_Color1.z, p_Color1.w),
            DirectX::SimpleMath::Vector2(p_TextureCoordinates1.x, p_TextureCoordinates1.y)
        ),
        DirectX::VertexPositionColorTexture(
            DirectX::SimpleMath::Vector3(p_V2.x, p_V2.y, p_V2.z), DirectX::SimpleMath::Vector4(p_Color2.x, p_Color2.y, p_Color2.z, p_Color2.w),
            DirectX::SimpleMath::Vector2(p_TextureCoordinates2.x, p_TextureCoordinates2.y)
        ),
        DirectX::VertexPositionColorTexture(
            DirectX::SimpleMath::Vector3(p_V3.x, p_V3.y, p_V3.z), DirectX::SimpleMath::Vector4(p_Color3.x, p_Color3.y, p_Color3.z, p_Color3.w),
            DirectX::SimpleMath::Vector2(p_TextureCoordinates3.x, p_TextureCoordinates3.y)
        )
    );
}

void DirectXRenderer::DrawQuad3D(
    const SVector3& p_V1, const SVector4& p_Color1, const SVector3& p_V2, const SVector4& p_Color2, const SVector3& p_V3, const SVector4& p_Color3,
    const SVector3& p_V4, const SVector4& p_Color4
)
{
    if (m_IsFrustumCullingEnabled && !IsPointInsideViewFrustum(p_V1) && !IsPointInsideViewFrustum(p_V2) && !IsPointInsideViewFrustum(p_V3)
        && !IsPointInsideViewFrustum(p_V4))
    {
        return;
    }

    m_PrimitiveBatch->DrawQuad(
        DirectX::VertexPositionColor(
            DirectX::SimpleMath::Vector3(p_V1.x, p_V1.y, p_V1.z), DirectX::SimpleMath::Vector4(p_Color1.x, p_Color1.y, p_Color1.z, p_Color1.w)
        ),
        DirectX::VertexPositionColor(
            DirectX::SimpleMath::Vector3(p_V2.x, p_V2.y, p_V2.z), DirectX::SimpleMath::Vector4(p_Color2.x, p_Color2.y, p_Color2.z, p_Color2.w)
        ),
        DirectX::VertexPositionColor(
            DirectX::SimpleMath::Vector3(p_V3.x, p_V3.y, p_V3.z), DirectX::SimpleMath::Vector4(p_Color3.x, p_Color3.y, p_Color3.z, p_Color3.w)
        ),
        DirectX::VertexPositionColor(
            DirectX::SimpleMath::Vector3(p_V4.x, p_V4.y, p_V4.z), DirectX::SimpleMath::Vector4(p_Color4.x, p_Color4.y, p_Color4.z, p_Color4.w)
        )
    );
}

void DirectXRenderer::DrawText2D(
    const ZString& p_Text, const SVector2& p_Position, const SVector4& p_Color, float p_Rotation, float p_Scale,
    HorizontalAlignment p_HorizontalAlignment, VerticalAlignment p_VerticalAlignment
)
{
    const std::string text(p_Text.ToCString(), p_Text.Length());
    const DirectX::SimpleMath::Vector2 stringSize = m_Font->MeasureString(text.c_str());

    DirectX::SimpleMath::Vector2 origin(0.f, 0.f);

    if (p_HorizontalAlignment == HorizontalAlignment::Center)
    {
        origin.x = stringSize.x / 2.f;
    }
    else if (p_HorizontalAlignment == HorizontalAlignment::Right)
    {
        origin.x = stringSize.x;
    }

    if (p_VerticalAlignment == VerticalAlignment::Middle)
    {
        origin.y = stringSize.y / 2.f;
    }
    else if (p_VerticalAlignment == VerticalAlignment::Bottom)
    {
        origin.y = stringSize.y;
    }

    m_Font->DrawString(
        m_SpriteBatch.get(), text.c_str(), DirectX::SimpleMath::Vector2(p_Position.x, p_Position.y),
        DirectX::SimpleMath::Vector4(p_Color.x, p_Color.y, p_Color.z, p_Color.w), p_Rotation, origin, p_Scale
    );
}

void DirectXRenderer::DrawText3D(
    const char* p_Text, const SMatrix& p_Transform, const SVector4& p_Color, float p_Scale, HorizontalAlignment p_HorizontalAlignment,
    VerticalAlignment p_VerticalAlignment, bool p_IsCameraTransform
)
{
    if (m_IsFrustumCullingEnabled && !IsPointInsideViewFrustum(p_Transform.Trans))
    {
        return;
    }

    int textLength = 0;

    while (p_Text[textLength] && textLength < 255)
    {
        textLength++;
    }

    int printableCharacterCount = 0;
    const char* p = p_Text;

    while (*p)
    {
        uint32_t cp = MDF_FONT::DecodeUTF8(p);

        if (MDF_FONT::HasGlyph(cp))
        {
            ++printableCharacterCount;
        }
    }

    MDF_FONT::STextBoundingBox textBoundingBox;

    MDF_FONT::CalcBoundingBox(textBoundingBox, p_Text);

    float offsetY = 0.f;

    if (p_VerticalAlignment == VerticalAlignment::Middle)
    {
        offsetY = -(textBoundingBox.m_fMaxY - textBoundingBox.m_fMinY) * 0.5f;
    }
    else if (p_VerticalAlignment == VerticalAlignment::Bottom)
    {
        offsetY = -(textBoundingBox.m_fMaxY - textBoundingBox.m_fMinY);
    }

    const float4 translate = float4(0.f, offsetY * p_Scale, 0.f, 1.f);
    const float4 scale = float4(p_Scale, p_Scale, p_Scale, 1.f);
    const SMatrix offsetMatrix = SMatrix::ScaleTranslate(scale, translate);

    SMatrix finalTransform = p_Transform;

    if (p_IsCameraTransform)
    {
        std::swap(finalTransform.YAxis, finalTransform.ZAxis);
    }

    finalTransform = finalTransform.AffineMultiply(offsetMatrix);

    std::vector<Triangle> triangles;

    triangles.reserve(2 * printableCharacterCount);

    std::istringstream inputStringStream(p_Text);
    std::string line;
    int lineIndex = 0;

    while (std::getline(inputStringStream, line))
    {
        MDF_FONT::STextBoundingBox textBoundingBox2;

        MDF_FONT::CalcBoundingBox(textBoundingBox2, line.c_str());

        float offsetX = 0.f;

        if (p_HorizontalAlignment == HorizontalAlignment::Center)
        {
            offsetX = -(textBoundingBox2.m_fMaxX - textBoundingBox2.m_fMinX) * 0.5f;
        }
        else if (p_HorizontalAlignment == HorizontalAlignment::Right)
        {
            offsetX = -(textBoundingBox2.m_fMaxX - textBoundingBox2.m_fMinX);
        }

        float penX = offsetX;
        float penY = -(lineIndex * MDF_FONT::g_LineHeight);

        const char* p = line.c_str();

        while (*p)
        {
            uint32_t codepoint = MDF_FONT::DecodeUTF8(p);

            if (codepoint == ' ')
            {
                penX += MDF_FONT::GetAdvanceWidth(codepoint);
                continue;
            }

            if (!MDF_FONT::HasGlyph(codepoint))
            {
                continue;
            }

            float vertices[8];
            float textureCoordinates[8];

            MDF_FONT::RenderQuad(codepoint, 1.0f, penX, penY, vertices, textureCoordinates);

            float4 bottomLeft = float4(vertices[0], 0.f, vertices[1], 1.f);
            float4 bottomRight = float4(vertices[2], 0.f, vertices[3], 1.f);
            float4 topRight = float4(vertices[4], 0.f, vertices[5], 1.f);
            float4 topLeft = float4(vertices[6], 0.f, vertices[7], 1.f);

            bottomLeft = finalTransform.WVectorTransform(bottomLeft);
            bottomRight = finalTransform.WVectorTransform(bottomRight);
            topRight = finalTransform.WVectorTransform(topRight);
            topLeft = finalTransform.WVectorTransform(topLeft);

            Triangle& triangle1 = triangles.emplace_back();
            Triangle& triangle2 = triangles.emplace_back();

            triangle1.m_Vertex1 = { { bottomLeft.x, bottomLeft.y, bottomLeft.z }, p_Color, { textureCoordinates[0], textureCoordinates[1] } };
            triangle1.m_Vertex2 = { { bottomRight.x, bottomRight.y, bottomRight.z }, p_Color, { textureCoordinates[2], textureCoordinates[3] } };
            triangle1.m_Vertex3 = { { topLeft.x, topLeft.y, topLeft.z }, p_Color, { textureCoordinates[6], textureCoordinates[7] } };

            triangle2.m_Vertex1 = { { bottomRight.x, bottomRight.y, bottomRight.z }, p_Color, { textureCoordinates[2], textureCoordinates[3] } };
            triangle2.m_Vertex2 = { { topRight.x, topRight.y, topRight.z }, p_Color, { textureCoordinates[4], textureCoordinates[5] } };
            triangle2.m_Vertex3 = { { topLeft.x, topLeft.y, topLeft.z }, p_Color, { textureCoordinates[6], textureCoordinates[7] } };
        }

        lineIndex++;
    }

    for (auto& triangle : triangles)
    {
        DrawTriangle3D(
            triangle.m_Vertex1.m_Position, triangle.m_Vertex1.m_Color, triangle.m_Vertex1.m_TexCoord, triangle.m_Vertex2.m_Position,
            triangle.m_Vertex2.m_Color, triangle.m_Vertex2.m_TexCoord, triangle.m_Vertex3.m_Position, triangle.m_Vertex3.m_Color,
            triangle.m_Vertex3.m_TexCoord
        );
    }
}

void DirectXRenderer::DrawMesh(const std::vector<SVector3>& p_Vertices, const std::vector<uint16_t>& p_Indices, const SVector4& p_VertexColor)
{
    if (p_Vertices.empty() || p_Indices.empty())
    {
        return;
    }

    if (m_IsFrustumCullingEnabled)
    {
        SVector3 min = p_Vertices[0];
        SVector3 max = p_Vertices[0];

        for (const auto& p_Vertex : p_Vertices)
        {
            min.x = std::min(min.x, p_Vertex.x);
            min.y = std::min(min.y, p_Vertex.y);
            min.z = std::min(min.z, p_Vertex.z);

            max.x = std::max(max.x, p_Vertex.x);
            max.y = std::max(max.y, p_Vertex.y);
            max.z = std::max(max.z, p_Vertex.z);
        }

        if (!IsAABBInsideViewFrustum(min, max, SMatrix()))
        {
            return;
        }
    }

    std::vector<DirectX::VertexPositionColor> vertices2;

    vertices2.reserve(p_Vertices.size());

    for (size_t i = 0; i < p_Vertices.size(); ++i)
    {
        const SVector3& vertex = p_Vertices[i];

        vertices2.push_back(
            DirectX::VertexPositionColor(
                DirectX::SimpleMath::Vector3(vertex.x, vertex.y, vertex.z),
                DirectX::SimpleMath::Vector4(p_VertexColor.x, p_VertexColor.y, p_VertexColor.z, p_VertexColor.w)
            )
        );
    }

    m_PrimitiveBatch->DrawIndexed(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, p_Indices.data(), p_Indices.size(), vertices2.data(), vertices2.size());
}

void DirectXRenderer::DrawMesh(
    ZRenderPrimitiveResource* p_RenderPrimitiveResource, ZRenderVertexBuffer** p_VertexBuffers, const uint32_t p_VertexBufferCount,
    ZRenderIndexBuffer* p_IndexBuffer, const SMatrix& p_Transform, const float4& p_PositionScale, const float4& p_PositionBias,
    const float4& p_TextureScaleBias, const SVector4& p_MaterialColor
)
{
    if (!p_RenderPrimitiveResource || !p_VertexBuffers || p_VertexBufferCount == 0 || !p_IndexBuffer)
    {
        return;
    }

    for (uint32_t i = 0; i < p_VertexBufferCount; ++i)
    {
        if (!p_VertexBuffers[i] || !p_VertexBuffers[i]->m_pBuffer)
        {
            return;
        }
    }

    const SVector3 rawCenter = (p_RenderPrimitiveResource->m_vMin + p_RenderPrimitiveResource->m_vMax) * 0.5f;
    const SVector3 rawExtents = (p_RenderPrimitiveResource->m_vMax - p_RenderPrimitiveResource->m_vMin) * 0.5f;

    const SVector3 center = rawCenter * SVector3(p_PositionScale) + SVector3(p_PositionBias);
    const SVector3 extents = rawExtents * SVector3(p_PositionScale);

    if (m_IsFrustumCullingEnabled)
    {
        const float4 worldCenter = p_Transform.WVectorTransform(float4(center, 1.f));

        if (!IsOBBInsideViewFrustum(worldCenter, float4(extents, 1.f), p_Transform))
        {
            return;
        }
    }

    std::vector<ID3D11Buffer*> vertexBuffers(p_VertexBufferCount);
    std::vector<UINT> strides(p_VertexBufferCount);
    std::vector<UINT> offsets(p_VertexBufferCount);

    for (uint32_t i = 0; i < p_VertexBufferCount; ++i)
    {
        vertexBuffers[i] = p_VertexBuffers[i]->m_pBuffer;
        strides[i] = p_VertexBuffers[i]->m_Description.nStride;
        offsets[i] = p_VertexBuffers[i]->m_nOffset;
    }

    ID3D11Device* device = Globals::RenderManager->m_pRenderDevice->m_pDirect3DDevice;
    ID3D11DeviceContext* immediateContext = Globals::RenderManager->m_pRenderDevice->m_pDeviceContextImmediate;

    m_DebugEffect->SetWorld(*reinterpret_cast<const DirectX::XMFLOAT4X4*>(&p_Transform));
    m_DebugEffect->SetPositionScale(*reinterpret_cast<const DirectX::XMFLOAT4*>(&p_PositionScale));
    m_DebugEffect->SetPositionBias(*reinterpret_cast<const DirectX::XMFLOAT4*>(&p_PositionBias));
    m_DebugEffect->SetTextureScaleBias(*reinterpret_cast<const DirectX::XMFLOAT4*>(&p_TextureScaleBias));
    m_DebugEffect->SetMaterialColor(*reinterpret_cast<const DirectX::XMFLOAT4*>(&p_MaterialColor));
    m_DebugEffect->Apply(immediateContext);

    immediateContext->IASetVertexBuffers(0, p_VertexBufferCount, vertexBuffers.data(), strides.data(), offsets.data());

    immediateContext->IASetIndexBuffer(p_IndexBuffer->m_pBuffer, DXGI_FORMAT_R16_UINT, p_IndexBuffer->m_nOffset);

    immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    const uint32_t indexCount = p_IndexBuffer->m_Description.nSize / p_IndexBuffer->m_Description.nStride;

    immediateContext->DrawIndexed(indexCount, 0, 0);
}

bool DirectXRenderer::IsPointInsideViewFrustum(const SVector3& p_Point) const
{
    return m_ViewFrustum.ContainsPoint(p_Point);
}

bool DirectXRenderer::IsAABBInsideViewFrustum(const SVector3& p_Min, const SVector3& p_Max, const SMatrix& p_Transform) const
{
    const DirectX::SimpleMath::Matrix world = *reinterpret_cast<const DirectX::SimpleMath::Matrix*>(&p_Transform);

    const AABB transformedAABB = TransformAABB(world, { p_Min, p_Max });

    return m_ViewFrustum.ContainsAABB(transformedAABB);
}

bool DirectXRenderer::IsOBBInsideViewFrustum(const float4& p_Center, const float4& p_HalfSize, const SMatrix& p_Transform) const
{
    return m_ViewFrustum.ContainsOBB(p_Transform, p_Center, p_HalfSize);
}

bool DirectXRenderer::IsSphereInsideViewFrustum(const SVector3& p_Center, float p_Radius) const
{
    return m_ViewFrustum.ContainsSphere(p_Center, p_Radius);
}

void DirectXRenderer::SetFrustumCullingEnabled(bool p_Enabled)
{
    m_IsFrustumCullingEnabled = p_Enabled;
}

bool DirectXRenderer::IsFrustumCullingEnabled() const
{
    return m_IsFrustumCullingEnabled;
}

void DirectXRenderer::SetDistanceCullingEnabled(bool p_Enabled)
{
    if (m_ViewFrustum.IsDistanceCullingEnabled() == p_Enabled)
    {
        return;
    }

    m_ViewFrustum.SetDistanceCullingEnabled(p_Enabled);

    m_ViewFrustum.UpdateClipPlanes(SMatrix(m_View), SMatrix(m_Projection));
}

bool DirectXRenderer::IsDistanceCullingEnabled() const
{
    return m_ViewFrustum.IsDistanceCullingEnabled();
}

void DirectXRenderer::SetMaxDrawDistance(float p_MaxDrawDistance)
{
    if (m_ViewFrustum.GetMaxDrawDistance() == p_MaxDrawDistance)
    {
        return;
    }

    m_ViewFrustum.SetMaxDrawDistance(p_MaxDrawDistance);

    m_ViewFrustum.UpdateClipPlanes(SMatrix(m_View), SMatrix(m_Projection));
}

float DirectXRenderer::GetMaxDrawDistance() const
{
    return m_ViewFrustum.GetMaxDrawDistance();
}

bool DirectXRenderer::CompileShaderFromString(
    const std::string& p_ShaderCode, const std::string& p_EntryPoint, const std::string& p_ShaderModel, ID3DBlob** p_ShaderBlob
)
{
    UINT compileFlags = 0;

#if defined(_DEBUG)
    compileFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    ScopedD3DRef<ID3DBlob> errorBlob;

    const HRESULT result = D3DCompile(
        p_ShaderCode.c_str(), p_ShaderCode.size(), nullptr, nullptr, nullptr, p_EntryPoint.c_str(), p_ShaderModel.c_str(), compileFlags, 0,
        p_ShaderBlob, &errorBlob.m_Ref
    );

    if (FAILED(result))
    {
        if (errorBlob)
        {
            Logger::Error("{}", static_cast<const char*>(errorBlob->GetBufferPointer()));
        }

        return false;
    }

    return true;
}

bool DirectXRenderer::CreateFontDistanceFieldTexture()
{
    ZRenderDevice* renderDevice = Globals::RenderManager->m_pRenderDevice;

    D3D11_TEXTURE2D_DESC textureDesc;
    ZeroMemory(&textureDesc, sizeof(textureDesc));
    textureDesc.Width = 1024;
    textureDesc.Height = 512;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED;

    D3D11_SUBRESOURCE_DATA initialData;
    initialData.pSysMem = MDF_FONT::g_DistanceField;
    initialData.SysMemPitch = 1024;
    initialData.SysMemSlicePitch = 1024 * 512;

    HRESULT result = renderDevice->m_pDirect3DDevice->CreateTexture2D(&textureDesc, &initialData, &m_FontDistanceFieldTexture.m_Ref);

    if (FAILED(result))
    {
        Logger::Error("Unable to create font distance field texture!");
        return false;
    }

    result = renderDevice->m_pDirect3DDevice->CreateShaderResourceView(m_FontDistanceFieldTexture, nullptr, &m_FontDistanceFieldSRV.m_Ref);

    if (FAILED(result))
    {
        Logger::Error("Unable to create SRV for distance field texture!");
        return false;
    }

    return true;
}

AABB DirectXRenderer::TransformAABB(const DirectX::SimpleMath::Matrix& p_Transform, const AABB& p_AABB)
{
    const DirectX::SimpleMath::Vector3 corners[8] = {
        { p_AABB.m_Min.x, p_AABB.m_Min.y, p_AABB.m_Min.z }, { p_AABB.m_Min.x, p_AABB.m_Min.y, p_AABB.m_Max.z },
        { p_AABB.m_Min.x, p_AABB.m_Max.y, p_AABB.m_Min.z }, { p_AABB.m_Min.x, p_AABB.m_Max.y, p_AABB.m_Max.z },
        { p_AABB.m_Max.x, p_AABB.m_Min.y, p_AABB.m_Min.z }, { p_AABB.m_Max.x, p_AABB.m_Min.y, p_AABB.m_Max.z },
        { p_AABB.m_Max.x, p_AABB.m_Max.y, p_AABB.m_Min.z }, { p_AABB.m_Max.x, p_AABB.m_Max.y, p_AABB.m_Max.z }
    };

    AABB transformedAABB = { { FLT_MAX, FLT_MAX, FLT_MAX }, { -FLT_MAX, -FLT_MAX, -FLT_MAX } };

    for (int32_t i = 0; i < 8; ++i)
    {
        const DirectX::SimpleMath::Vector3 transformedCorner = DirectX::SimpleMath::Vector3::Transform(corners[i], p_Transform);

        transformedAABB.m_Min.x = std::min(transformedAABB.m_Min.x, transformedCorner.x);
        transformedAABB.m_Min.y = std::min(transformedAABB.m_Min.y, transformedCorner.y);
        transformedAABB.m_Min.z = std::min(transformedAABB.m_Min.z, transformedCorner.z);

        transformedAABB.m_Max.x = std::max(transformedAABB.m_Max.x, transformedCorner.x);
        transformedAABB.m_Max.y = std::max(transformedAABB.m_Max.y, transformedCorner.y);
        transformedAABB.m_Max.z = std::max(transformedAABB.m_Max.z, transformedCorner.z);
    }

    return transformedAABB;
}

DEFINE_THISCALL_DETOUR_WITH_CONTEXT(
    DirectXRenderer, void, ZRenderGraphNodeRenderer_DrawOSDAndScaleform, ZRenderGraphNodeRenderer* p_RenderGraphNodeRenderer,
    ZRenderContext* p_RenderContext, ZRenderTargetView* p_RTV, ZRenderDepthStencilView* p_DSV, TArray<ZRenderGraphNode*>& p_OSDNodes,
    ZRenderGraphTraversal* p_RenderGraphTraversal
)
{
    if (Globals::ScaleformManager->m_bIsInMainMenu || Globals::HUDManager->m_bPauseMenuActive || !Globals::LevelManager->m_rHitman)
    {
        return { HookAction::Continue() };
    }

    m_View = *reinterpret_cast<DirectX::FXMMATRIX*>(&p_RenderContext->m_mWorldToView);
    m_Projection = *reinterpret_cast<DirectX::FXMMATRIX*>(&p_RenderContext->m_mViewToProjection);

    m_ViewProjection = m_View * m_Projection;
    m_ProjectionViewInverse = (m_Projection * m_View).Invert();

    m_ViewFrustum.UpdateClipPlanes(p_RenderContext->m_mWorldToView, p_RenderContext->m_mViewToProjection);

    m_PrimitiveEffect->SetView(m_View);
    m_PrimitiveEffect->SetProjection(m_Projection);

    m_TextEffect->SetView(m_View);
    m_TextEffect->SetProjection(m_Projection);

    m_DebugEffect->SetView(m_View);
    m_DebugEffect->SetProjection(m_Projection);

    DepthDraw(p_RTV, p_DSV);
    Draw(p_RTV);

    return { HookAction::Continue() };
}
