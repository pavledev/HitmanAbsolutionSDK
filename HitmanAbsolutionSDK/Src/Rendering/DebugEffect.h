#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <string>

#include "directxtk/CommonStates.h"

#include <Glacier/ZMath.h>

class DebugEffect
{
  public:
    struct Constants
    {
        DirectX::XMFLOAT4X4 m_World;
        DirectX::XMFLOAT4X4 m_View;
        DirectX::XMFLOAT4X4 m_Projection;

        DirectX::XMFLOAT4 m_PositionScale;
        DirectX::XMFLOAT4 m_PositionBias;
        DirectX::XMFLOAT4 m_TextureScaleBias;

        DirectX::XMFLOAT4 m_MaterialColor;
    };

    static_assert((sizeof(Constants) % 16) == 0, "Constant buffer size isn't padded correctly!");

    DebugEffect(ID3D11Device* p_Device, const D3D11_INPUT_ELEMENT_DESC* p_InputElements, uint32_t p_InputElementCount);

    ~DebugEffect();

    void CreateTexture(ID3D11Device* p_Device);
    void CreateSampler(ID3D11Device* p_Device);

    void Apply(ID3D11DeviceContext* p_DeviceContext);

    void SetWorld(const DirectX::XMFLOAT4X4& p_World);
    void SetView(const DirectX::XMFLOAT4X4& p_View);
    void SetProjection(const DirectX::XMFLOAT4X4& p_Projection);
    void SetPositionScale(const DirectX::XMFLOAT4& p_PositionScale);
    void SetPositionBias(const DirectX::XMFLOAT4& p_PositionBias);
    void SetTextureScaleBias(const DirectX::XMFLOAT4& p_TextureScaleBias);
    void SetMaterialColor(const DirectX::XMFLOAT4& p_MaterialColor);

    bool CompileShaderFromString(
        const std::string& p_ShaderCode, const std::string& p_EntryPoint, const std::string& p_ShaderModel, ID3DBlob** p_ShaderBlob
    );

  private:
    Constants m_Constants{};

    ID3D11VertexShader* m_pVertexShader = nullptr;
    ID3D11PixelShader* m_pPixelShader = nullptr;
    ID3D11InputLayout* m_pInputLayout = nullptr;

    ID3D11Buffer* m_pConstantBuffer = nullptr;

    ID3D11Texture2D* m_pTexture = nullptr;
    ID3D11ShaderResourceView* m_pTextureSRV = nullptr;
    ID3D11SamplerState* m_pSampler = nullptr;

    std::unique_ptr<DirectX::CommonStates> m_CommonStates;
};
