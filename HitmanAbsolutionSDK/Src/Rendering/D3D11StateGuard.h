#pragma once

#include <d3d11.h>

class D3D11StateGuard
{
  public:
    explicit D3D11StateGuard(ID3D11DeviceContext* p_Context) : m_Context(p_Context)
    {
        // Input Assembler
        m_Context->IAGetInputLayout(&m_InputLayout);
        m_Context->IAGetPrimitiveTopology(&m_PrimitiveTopology);

        m_Context->IAGetVertexBuffers(0, 1, &m_VertexBuffer, &m_VertexBufferStride, &m_VertexBufferOffset);

        m_Context->IAGetIndexBuffer(&m_IndexBuffer, &m_IndexBufferFormat, &m_IndexBufferOffset);

        // Vertex Shader
        m_Context->VSGetShader(&m_VertexShader, nullptr, nullptr);

        m_Context->VSGetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, m_VSConstantBuffers);

        m_Context->VSGetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, m_VSShaderResources);

        m_Context->VSGetSamplers(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT, m_VSSamplers);

        // Pixel Shader
        m_Context->PSGetShader(&m_PixelShader, nullptr, nullptr);

        m_Context->PSGetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, m_PSConstantBuffers);

        m_Context->PSGetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, m_PSShaderResources);

        m_Context->PSGetSamplers(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT, m_PSSamplers);

        // Rasterizer
        m_Context->RSGetState(&m_RasterizerState);

        m_ViewportCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
        m_Context->RSGetViewports(&m_ViewportCount, m_Viewports);

        m_ScissorRectCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
        m_Context->RSGetScissorRects(&m_ScissorRectCount, m_ScissorRects);

        // Output Merger
        m_Context->OMGetBlendState(&m_BlendState, m_BlendFactor, &m_SampleMask);

        m_Context->OMGetDepthStencilState(&m_DepthStencilState, &m_StencilRef);

        m_Context->OMGetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, m_RenderTargetViews, &m_DepthStencilView);
    }

    ~D3D11StateGuard()
    {
        Restore();
        Release();
    }

    D3D11StateGuard(const D3D11StateGuard&) = delete;
    D3D11StateGuard& operator=(const D3D11StateGuard&) = delete;

  private:
    void Restore()
    {
        // Input Assembler
        m_Context->IASetInputLayout(m_InputLayout);
        m_Context->IASetPrimitiveTopology(m_PrimitiveTopology);

        m_Context->IASetVertexBuffers(0, 1, &m_VertexBuffer, &m_VertexBufferStride, &m_VertexBufferOffset);

        m_Context->IASetIndexBuffer(m_IndexBuffer, m_IndexBufferFormat, m_IndexBufferOffset);

        // Vertex Shader
        m_Context->VSSetShader(m_VertexShader, nullptr, 0);

        m_Context->VSSetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, m_VSConstantBuffers);

        m_Context->VSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, m_VSShaderResources);

        m_Context->VSSetSamplers(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT, m_VSSamplers);

        // Pixel Shader
        m_Context->PSSetShader(m_PixelShader, nullptr, 0);

        m_Context->PSSetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, m_PSConstantBuffers);

        m_Context->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, m_PSShaderResources);

        m_Context->PSSetSamplers(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT, m_PSSamplers);

        // Rasterizer
        m_Context->RSSetState(m_RasterizerState);
        m_Context->RSSetViewports(m_ViewportCount, m_Viewports);
        m_Context->RSSetScissorRects(m_ScissorRectCount, m_ScissorRects);

        // Output Merger
        m_Context->OMSetBlendState(m_BlendState, m_BlendFactor, m_SampleMask);

        m_Context->OMSetDepthStencilState(m_DepthStencilState, m_StencilRef);

        m_Context->OMSetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, m_RenderTargetViews, m_DepthStencilView);
    }

    void Release()
    {
        ReleaseObject(m_InputLayout);
        ReleaseObject(m_VertexBuffer);
        ReleaseObject(m_IndexBuffer);

        ReleaseObject(m_VertexShader);

        ReleaseObjects(m_VSConstantBuffers);
        ReleaseObjects(m_VSShaderResources);
        ReleaseObjects(m_VSSamplers);

        ReleaseObject(m_PixelShader);

        ReleaseObjects(m_PSConstantBuffers);
        ReleaseObjects(m_PSShaderResources);
        ReleaseObjects(m_PSSamplers);

        ReleaseObject(m_RasterizerState);

        ReleaseObject(m_BlendState);
        ReleaseObject(m_DepthStencilState);

        ReleaseObjects(m_RenderTargetViews);
        ReleaseObject(m_DepthStencilView);
    }

    template<typename T> static void ReleaseObject(T*& p_Object)
    {
        if (p_Object)
        {
            p_Object->Release();
            p_Object = nullptr;
        }
    }

    template<typename T, size_t N> static void ReleaseObjects(T* (&p_Objects)[N])
    {
        for (T*& object : p_Objects)
        {
            ReleaseObject(object);
        }
    }

    ID3D11DeviceContext* m_Context = nullptr;

    // Input Assembler
    ID3D11InputLayout* m_InputLayout = nullptr;
    D3D11_PRIMITIVE_TOPOLOGY m_PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;

    ID3D11Buffer* m_VertexBuffer = nullptr;
    UINT m_VertexBufferStride = 0;
    UINT m_VertexBufferOffset = 0;

    ID3D11Buffer* m_IndexBuffer = nullptr;
    DXGI_FORMAT m_IndexBufferFormat = DXGI_FORMAT_UNKNOWN;
    UINT m_IndexBufferOffset = 0;

    // Vertex Shader
    ID3D11VertexShader* m_VertexShader = nullptr;

    ID3D11Buffer* m_VSConstantBuffers[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT]{};
    ID3D11ShaderResourceView* m_VSShaderResources[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT]{};
    ID3D11SamplerState* m_VSSamplers[D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT]{};

    // Pixel Shader
    ID3D11PixelShader* m_PixelShader = nullptr;

    ID3D11Buffer* m_PSConstantBuffers[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT]{};
    ID3D11ShaderResourceView* m_PSShaderResources[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT]{};
    ID3D11SamplerState* m_PSSamplers[D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT]{};

    // Rasterizer
    ID3D11RasterizerState* m_RasterizerState = nullptr;

    UINT m_ViewportCount = 0;
    D3D11_VIEWPORT m_Viewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE]{};

    UINT m_ScissorRectCount = 0;
    D3D11_RECT m_ScissorRects[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE]{};

    // Output Merger
    ID3D11BlendState* m_BlendState = nullptr;
    FLOAT m_BlendFactor[4]{};
    UINT m_SampleMask = 0;

    ID3D11DepthStencilState* m_DepthStencilState = nullptr;
    UINT m_StencilRef = 0;

    ID3D11RenderTargetView* m_RenderTargetViews[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT]{};
    ID3D11DepthStencilView* m_DepthStencilView = nullptr;
};
