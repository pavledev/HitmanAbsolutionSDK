#pragma once

#include <d3d11.h>
#include <functional>
#include <stdexcept>
#include <cassert>

#include <DirectXHelpers.h>

#include "Utils/D3DUtils.h"

template<typename TVertex> class CustomPrimitiveBatch
{
  private:
    using FlushBatchCallback = std::function<void()>;

  public:
    CustomPrimitiveBatch(
        ID3D11DeviceContext* p_DeviceContext, FlushBatchCallback p_FlushBatchCallback, size_t p_MaxIndices = m_DefaultBatchSize * 3,
        size_t p_MaxVertices = m_DefaultBatchSize, size_t p_VertexSize = sizeof(TVertex)
    )
        : m_DeviceContext(p_DeviceContext)
        , m_FlushBatchCallback(p_FlushBatchCallback)
        , m_MaxIndices(p_MaxIndices)
        , m_MaxVertices(p_MaxVertices)
        , m_VertexSize(p_VertexSize)
        , m_CurrentTopology(D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED)
        , m_InBeginEndPair(false)
        , m_CurrentlyIndexed(false)
        , m_CurrentIndex(0)
        , m_CurrentVertex(0)
        , m_BaseIndex(0)
        , m_BaseVertex(0)
        , m_MappedIndices{}
        , m_MappedVertices{}
    {
        if (!p_DeviceContext)
        {
            throw std::invalid_argument("Direct3D device context is null");
        }

        ScopedD3DRef<ID3D11Device> device;
        p_DeviceContext->GetDevice(&device.m_Ref);

        if (!p_MaxVertices)
        {
            throw std::invalid_argument("p_MaxVertices must be greater than 0");
        }

        if (p_VertexSize > D3D11_REQ_MULTI_ELEMENT_STRUCTURE_SIZE_IN_BYTES)
        {
            throw std::invalid_argument("Vertex size is too large for DirectX 11");
        }

        const uint64_t ibBytes = uint64_t(p_MaxIndices) * sizeof(uint16_t);
        if (ibBytes > uint64_t(D3D11_REQ_RESOURCE_SIZE_IN_MEGABYTES_EXPRESSION_A_TERM * 1024u * 1024u) || ibBytes > UINT32_MAX)
        {
            throw std::invalid_argument("IB too large for DirectX 11");
        }

        const uint64_t vbBytes = uint64_t(p_MaxVertices) * uint64_t(p_VertexSize);
        if (vbBytes > uint64_t(D3D11_REQ_RESOURCE_SIZE_IN_MEGABYTES_EXPRESSION_A_TERM * 1024u * 1024u) || vbBytes > UINT32_MAX)
        {
            throw std::invalid_argument("VB too large for DirectX 11");
        }

        if (p_MaxIndices > 0)
        {
            CreateDynamicBuffer(device.m_Ref, static_cast<uint32_t>(ibBytes), D3D11_BIND_INDEX_BUFFER, &m_IndexBuffer.m_Ref);
        }

        CreateDynamicBuffer(device.m_Ref, static_cast<uint32_t>(vbBytes), D3D11_BIND_VERTEX_BUFFER, &m_VertexBuffer.m_Ref);
    }

    CustomPrimitiveBatch(CustomPrimitiveBatch&&) = default;
    CustomPrimitiveBatch& operator=(CustomPrimitiveBatch&&) = default;

    CustomPrimitiveBatch(CustomPrimitiveBatch const&) = delete;
    CustomPrimitiveBatch& operator=(CustomPrimitiveBatch const&) = delete;

    void Begin()
    {
        if (m_InBeginEndPair)
        {
            throw std::logic_error("Cannot nest Begin calls");
        }

        // If this is a deferred D3D context, reset position so the first Map calls will use D3D11_MAP_WRITE_DISCARD.
        if (m_DeviceContext->GetType() == D3D11_DEVICE_CONTEXT_DEFERRED)
        {
            m_CurrentIndex = 0;
            m_CurrentVertex = 0;
        }

        m_InBeginEndPair = true;
    }

    void End()
    {
        if (!m_InBeginEndPair)
        {
            throw std::logic_error("Begin must be called before End");
        }

        FlushBatch();

        m_InBeginEndPair = false;
    }

    void Draw(
        D3D11_PRIMITIVE_TOPOLOGY p_Topology, bool p_IsIndexed, uint16_t const* p_Indices, size_t p_IndexCount, size_t p_VertexCount,
        void** p_MappedVertices
    )
    {
        if (p_IsIndexed && !p_Indices)
        {
            throw std::invalid_argument("Indices cannot be null");
        }

        if (p_IndexCount >= m_MaxIndices)
        {
            throw std::out_of_range("Too many indices");
        }

        if (p_VertexCount >= m_MaxVertices)
        {
            throw std::out_of_range("Too many vertices");
        }

        if (!m_InBeginEndPair)
        {
            throw std::logic_error("Begin must be called before Draw");
        }

        const bool wrapIndexBuffer = m_CurrentIndex + p_IndexCount > m_MaxIndices;
        const bool wrapVertexBuffer = m_CurrentVertex + p_VertexCount > m_MaxVertices;

        if ((p_Topology != m_CurrentTopology) || (p_IsIndexed != m_CurrentlyIndexed) || wrapIndexBuffer || wrapVertexBuffer)
        {
            FlushBatch();
        }

        if (wrapIndexBuffer)
        {
            m_CurrentIndex = 0;
        }

        if (wrapVertexBuffer)
        {
            m_CurrentVertex = 0;
        }

        if (m_CurrentTopology == D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED)
        {
            if (p_IsIndexed)
            {
                LockBuffer(m_DeviceContext.m_Ref, m_IndexBuffer.m_Ref, m_CurrentIndex, &m_BaseIndex, &m_MappedIndices);
            }

            LockBuffer(m_DeviceContext.m_Ref, m_VertexBuffer.m_Ref, m_CurrentVertex, &m_BaseVertex, &m_MappedVertices);

            m_CurrentTopology = p_Topology;
            m_CurrentlyIndexed = p_IsIndexed;
        }

        if (p_IsIndexed)
        {
            auto outputIndices = static_cast<uint16_t*>(m_MappedIndices.pData) + m_CurrentIndex;

            for (size_t i = 0; i < p_IndexCount; i++)
            {
                outputIndices[i] = static_cast<uint16_t>(p_Indices[i] + m_CurrentVertex - m_BaseVertex);
            }

            m_CurrentIndex += p_IndexCount;
        }

        *p_MappedVertices = static_cast<uint8_t*>(m_MappedVertices.pData) + (m_CurrentVertex * m_VertexSize);

        m_CurrentVertex += p_VertexCount;
    }

    void Draw(D3D11_PRIMITIVE_TOPOLOGY p_Topology, TVertex const* p_Vertices, size_t p_VertexCount)
    {
        void* mappedVertices;
        Draw(p_Topology, false, nullptr, 0, p_VertexCount, &mappedVertices);

        memcpy(mappedVertices, p_Vertices, p_VertexCount * sizeof(TVertex));
    }

    void
    DrawIndexed(D3D11_PRIMITIVE_TOPOLOGY p_Topology, uint16_t const* p_Indices, size_t indexCount, TVertex const* p_Vertices, size_t p_VertexCount)
    {
        void* mappedVertices;
        Draw(p_Topology, true, p_Indices, indexCount, p_VertexCount, &mappedVertices);

        memcpy(mappedVertices, p_Vertices, p_VertexCount * sizeof(TVertex));
    }

    void DrawLine(const TVertex& p_V1, const TVertex& p_V2)
    {
        TVertex* mappedVertices;
        Draw(D3D11_PRIMITIVE_TOPOLOGY_LINELIST, false, nullptr, 0, 2, reinterpret_cast<void**>(&mappedVertices));

        mappedVertices[0] = p_V1;
        mappedVertices[1] = p_V2;
    }

    void DrawTriangle(const TVertex& p_V1, const TVertex& p_V2, const TVertex& p_V3)
    {
        TVertex* mappedVertices;
        Draw(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, false, nullptr, 0, 3, reinterpret_cast<void**>(&mappedVertices));

        mappedVertices[0] = p_V1;
        mappedVertices[1] = p_V2;
        mappedVertices[2] = p_V3;
    }

    void DrawQuad(const TVertex& p_V1, const TVertex& p_V2, const TVertex& p_V3, const TVertex& p_V4)
    {
        static const uint16_t quadIndices[] = { 0, 1, 2, 0, 2, 3 };
        TVertex* mappedVertices;
        Draw(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, true, quadIndices, 6, 4, reinterpret_cast<void**>(&mappedVertices));

        mappedVertices[0] = p_V1;
        mappedVertices[1] = p_V2;
        mappedVertices[2] = p_V3;
        mappedVertices[3] = p_V4;
    }

  private:
    void FlushBatch()
    {
        if (m_CurrentTopology == D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED)
        {
            return;
        }

        m_DeviceContext->Unmap(m_VertexBuffer.m_Ref, 0);

        if (m_CurrentlyIndexed)
        {
            m_DeviceContext->Unmap(m_IndexBuffer.m_Ref, 0);
        }

        m_FlushBatchCallback();

        const UINT vertexStride = static_cast<UINT>(m_VertexSize);
        constexpr UINT vertexOffset = 0;

        ID3D11Buffer* vertexBuffer = m_VertexBuffer.m_Ref;

        m_DeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexStride, &vertexOffset);

        if (m_CurrentlyIndexed)
        {
            m_DeviceContext->IASetIndexBuffer(m_IndexBuffer.m_Ref, DXGI_FORMAT_R16_UINT, 0);
        }

        m_DeviceContext->IASetPrimitiveTopology(m_CurrentTopology);

        if (m_CurrentlyIndexed)
        {
            m_DeviceContext->DrawIndexed(
                static_cast<UINT>(m_CurrentIndex - m_BaseIndex), static_cast<UINT>(m_BaseIndex), static_cast<INT>(m_BaseVertex)
            );
        }
        else
        {
            m_DeviceContext->Draw(static_cast<UINT>(m_CurrentVertex - m_BaseVertex), static_cast<UINT>(m_BaseVertex));
        }

        m_CurrentTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
    }

    static void CreateDynamicBuffer(ID3D11Device* p_Device, uint32_t p_BufferSize, D3D11_BIND_FLAG p_BindFlag, ID3D11Buffer** p_Buffer)
    {
        D3D11_BUFFER_DESC description = {};
        description.ByteWidth = p_BufferSize;
        description.BindFlags = p_BindFlag;
        description.Usage = D3D11_USAGE_DYNAMIC;
        description.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        const HRESULT result = p_Device->CreateBuffer(&description, nullptr, p_Buffer);

        if (FAILED(result))
        {
            throw std::runtime_error("Failed to create Direct3D 11 buffer.");
        }

        assert(p_Buffer != nullptr && *p_Buffer != nullptr);
    }

    static void LockBuffer(
        ID3D11DeviceContext* p_DeviceContext, ID3D11Buffer* p_Buffer, size_t p_CurrentPosition, size_t* p_BasePosition,
        D3D11_MAPPED_SUBRESOURCE* p_MappedResource
    )
    {
        const D3D11_MAP mapType = (p_CurrentPosition == 0) ? D3D11_MAP_WRITE_DISCARD : D3D11_MAP_WRITE_NO_OVERWRITE;

        const HRESULT result = p_DeviceContext->Map(p_Buffer, 0, mapType, 0, p_MappedResource);

        if (FAILED(result))
        {
            throw std::runtime_error("Failed to map Direct3D 11 buffer.");
        }

        *p_BasePosition = p_CurrentPosition;
    }

    FlushBatchCallback m_FlushBatchCallback;

    static constexpr size_t m_DefaultBatchSize = 4096;

    ScopedD3DRef<ID3D11DeviceContext> m_DeviceContext;
    ScopedD3DRef<ID3D11Buffer> m_IndexBuffer;
    ScopedD3DRef<ID3D11Buffer> m_VertexBuffer;

    size_t m_MaxIndices;
    size_t m_MaxVertices;
    size_t m_VertexSize;

    D3D11_PRIMITIVE_TOPOLOGY m_CurrentTopology;
    bool m_InBeginEndPair;
    bool m_CurrentlyIndexed;

    size_t m_CurrentIndex;
    size_t m_CurrentVertex;

    size_t m_BaseIndex;
    size_t m_BaseVertex;

    D3D11_MAPPED_SUBRESOURCE m_MappedIndices;
    D3D11_MAPPED_SUBRESOURCE m_MappedVertices;
};
