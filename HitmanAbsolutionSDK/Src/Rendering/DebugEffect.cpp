#include <d3dcompiler.h>

#include "DebugEffect.h"

#include "Logging.h"
#include "Utils/D3DUtils.h"
#include "Logging.h"

DebugEffect::DebugEffect(ID3D11Device* p_Device, const D3D11_INPUT_ELEMENT_DESC* p_InputElements, const uint32_t p_InputElementCount)
{
    const std::string debugVertexShader = R"(
        struct DebugEffectConstants
        {
            row_major float4x4 world;
            row_major float4x4 view;
            row_major float4x4 projection;

            float4 positionScale;
            float4 positionBias;
            float4 textureScaleBias;

            float4 materialColor;
        };

        cbuffer DebugConstants : register(b0)
        {
            DebugEffectConstants gConstants;
        };

        struct VSInput
        {
            float4 position : POSITION0;
            float4 normal   : NORMAL0;
            float4 tangent  : TANGENT0;
            float4 binormal : BINORMAL0;
            float2 texcoord : TEXCOORD0;
            float4 color    : COLOR0;
        };

        struct VSOutput
        {
            float4 position : SV_POSITION;
            float4 color    : COLOR0;
            float2 texcoord : TEXCOORD0;
        };

        VSOutput main(VSInput input)
        {
            VSOutput output;

            float4 localPos = float4(input.position.xyz, 1.0f);
            localPos.xyz = localPos.xyz * gConstants.positionScale.xyz + gConstants.positionBias.xyz;

            float4 worldPos = mul(localPos, gConstants.world);
            float4 viewPos = mul(worldPos, gConstants.view);
            output.position = mul(viewPos, gConstants.projection);

            output.color = input.color;

            float2 scaledUV = input.texcoord * gConstants.textureScaleBias.xy + gConstants.textureScaleBias.zw;
            output.texcoord = scaledUV;

            return output;
        }
    )";

    const std::string debugPixelShader = R"(
        struct DebugEffectConstants
        {
            row_major float4x4 world;
            row_major float4x4 view;
            row_major float4x4 projection;

            float4 positionScale;
            float4 positionBias;
            float4 textureScaleBias;

            float4 materialColor;
        };

        cbuffer DebugConstants : register(b0)
        {
            DebugEffectConstants gConstants;
        };

        SamplerState samplerPointClampNode_s : register(s0);
        Texture2D<float4> mapDebug2D : register(t0);

        struct PSInput
        {
            float4 position : SV_POSITION;
            float4 color    : COLOR;
            float2 texcoord : TEXCOORD;
        };

        float4 main(PSInput input) : SV_TARGET
        {
            float4 texColor = mapDebug2D.Sample(samplerPointClampNode_s, input.texcoord);
            return gConstants.materialColor * texColor * input.color;
        }
    )";

    ScopedD3DRef<ID3DBlob> vertexShaderBlob;
    ScopedD3DRef<ID3DBlob> pixelShaderBlob;

    if (!CompileShaderFromString(debugVertexShader, "main", "vs_5_0", &vertexShaderBlob.m_Ref))
    {
        return;
    }

    if (!CompileShaderFromString(debugPixelShader, "main", "ps_5_0", &pixelShaderBlob.m_Ref))
    {
        return;
    }

    HRESULT result = p_Device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), nullptr, &m_pVertexShader);

    if (FAILED(result))
    {
        Logger::Error("Failed to create vertex shader!");
        return;
    }

    result = p_Device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), nullptr, &m_pPixelShader);

    if (FAILED(result))
    {
        Logger::Error("Failed to create pixel shader!");
        return;
    }

    result = p_Device->CreateInputLayout(
        p_InputElements, p_InputElementCount, vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &m_pInputLayout
    );

    if (FAILED(result))
    {
        Logger::Error("Failed to create input layout!");
        return;
    }

    D3D11_BUFFER_DESC constantBufferDesc = {};
    constantBufferDesc.ByteWidth = sizeof(Constants);
    constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    result = p_Device->CreateBuffer(&constantBufferDesc, nullptr, &m_pConstantBuffer);

    if (FAILED(result))
    {
        Logger::Error("Failed to create constant buffer!");
        return;
    }

    m_CommonStates = std::make_unique<DirectX::CommonStates>(p_Device);

    CreateTexture(p_Device);
    CreateSampler(p_Device);
}

DebugEffect::~DebugEffect()
{
    if (m_pSampler)
    {
        m_pSampler->Release();
    }

    if (m_pTextureSRV)
    {
        m_pTextureSRV->Release();
    }

    if (m_pTexture)
    {
        m_pTexture->Release();
    }

    if (m_pConstantBuffer)
    {
        m_pConstantBuffer->Release();
    }

    if (m_pInputLayout)
    {
        m_pInputLayout->Release();
    }

    if (m_pPixelShader)
    {
        m_pPixelShader->Release();
    }

    if (m_pVertexShader)
    {
        m_pVertexShader->Release();
    }
}

void DebugEffect::CreateTexture(ID3D11Device* p_Device)
{
    constexpr uint32_t whitePixel = 0xFFFFFFFF;

    D3D11_TEXTURE2D_DESC textureDesc = {};
    textureDesc.Width = 1;
    textureDesc.Height = 1;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = &whitePixel;
    initData.SysMemPitch = sizeof(whitePixel);
    initData.SysMemSlicePitch = sizeof(whitePixel);

    HRESULT result = p_Device->CreateTexture2D(&textureDesc, &initData, &m_pTexture);

    if (FAILED(result))
    {
        Logger::Error("Failed to create DebugEffect texture!");
        return;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = textureDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;

    result = p_Device->CreateShaderResourceView(m_pTexture, &srvDesc, &m_pTextureSRV);

    if (FAILED(result))
    {
        Logger::Error("Failed to create DebugEffect shader resource view!");
    }
}

void DebugEffect::CreateSampler(ID3D11Device* p_Device)
{
    D3D11_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.MipLODBias = 0.f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.MinLOD = 0.f;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    const HRESULT result = p_Device->CreateSamplerState(&samplerDesc, &m_pSampler);

    if (FAILED(result))
    {
        Logger::Error("Failed to create DebugEffect sampler!");
    }
}

void DebugEffect::Apply(ID3D11DeviceContext* p_DeviceContext)
{
    D3D11_MAPPED_SUBRESOURCE mappedResource = {};

    const HRESULT result = p_DeviceContext->Map(m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

    if (FAILED(result))
    {
        return;
    }

    std::memcpy(mappedResource.pData, &m_Constants, sizeof(m_Constants));

    p_DeviceContext->Unmap(m_pConstantBuffer, 0);

    p_DeviceContext->IASetInputLayout(m_pInputLayout);

    p_DeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
    p_DeviceContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);

    p_DeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
    p_DeviceContext->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);
    p_DeviceContext->PSSetShaderResources(0, 1, &m_pTextureSRV);
    p_DeviceContext->PSSetSamplers(0, 1, &m_pSampler);

    p_DeviceContext->RSSetState(m_CommonStates->CullNone());

    p_DeviceContext->OMSetDepthStencilState(m_CommonStates->DepthRead(), 0);

    constexpr float blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };

    p_DeviceContext->OMSetBlendState(m_CommonStates->Opaque(), blendFactor, 0xFFFFFFFF);
}

void DebugEffect::SetWorld(const DirectX::XMFLOAT4X4& p_World)
{
    m_Constants.m_World = p_World;
}

void DebugEffect::SetView(const DirectX::XMFLOAT4X4& p_View)
{
    m_Constants.m_View = p_View;
}

void DebugEffect::SetProjection(const DirectX::XMFLOAT4X4& p_Projection)
{
    m_Constants.m_Projection = p_Projection;
}

void DebugEffect::SetPositionScale(const DirectX::XMFLOAT4& p_PositionScale)
{
    m_Constants.m_PositionScale = p_PositionScale;
}

void DebugEffect::SetPositionBias(const DirectX::XMFLOAT4& p_PositionBias)
{
    m_Constants.m_PositionBias = p_PositionBias;
}

void DebugEffect::SetTextureScaleBias(const DirectX::XMFLOAT4& p_TextureScaleBias)
{
    m_Constants.m_TextureScaleBias = p_TextureScaleBias;
}

void DebugEffect::SetMaterialColor(const DirectX::XMFLOAT4& p_MaterialColor)
{
    m_Constants.m_MaterialColor = p_MaterialColor;
}

bool DebugEffect::CompileShaderFromString(
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
