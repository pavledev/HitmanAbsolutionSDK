#pragma once

#include <d3d11.h>

#include "ZRenderDeviceBase.h"
#include "Utility/Alignment.h"
#include "ZRenderSwapChain.h"

struct SRenderDestinationDesc;
struct SRenderViewport;
enum ERenderPrimitiveTopology;
struct float4;
class ZRenderShader;
class ZRenderConstBufferRef;
class ZRenderVertexBuffer;
class ZRenderIndexBuffer;
class ZRenderInputLayout;
class ZRenderBlendState;
class ZRenderRasterizerState;
class ZRenderDepthStencilState;
class ZRenderTargetView3D;
class ZRenderTargetView;
class ZRenderDepthStencilView;
struct SRenderDeviceCaps;

class alignas(8) ZRenderDevice : public ZRenderDeviceBase
{
public:
	virtual ~ZRenderDevice() = 0;
	virtual ZRenderTexture2D* CreateTexture2D(const SRenderTexture2DDesc* pDescription, const SRenderSubResourceData* pData) = 0;
	virtual ZRenderTexture3D* CreateTexture3D(const SRenderTexture3DDesc* pDescription, const SRenderSubResourceData* pData) = 0;
	virtual void SetGamma(float fGamma) = 0;
	virtual ZRenderSwapChain* GetSwapChain();
	virtual void Reset(const SRenderDestinationDesc* pDescription, bool bForce) = 0;
	virtual bool Recover() = 0;
	virtual void GetInternalParameters(void** parameter1, void** parameter2) = 0;
	virtual void GetViewport(SRenderViewport* pViewport) = 0;
	virtual void SetViewport(const SRenderViewport& vp) = 0;
	virtual bool BeginScene() = 0;
	virtual void EndScene() = 0;
	virtual void Present() = 0;
	virtual void ApplyCustomGammaCorrection() = 0;
	virtual void SetPrimitiveTopology(ERenderPrimitiveTopology ePrimitiveTopology) = 0;
	virtual void DrawIndexed(unsigned int nStartIndex, unsigned int nNumIndices, int nBaseVertex, unsigned int nNumVertices) = 0;
	virtual void DrawIndexedInstanced(unsigned int nIndexCountPerInstance, unsigned int nInstanceCount, unsigned int nStartIndex, int nBaseVertex, unsigned int nStartInstanceLocation, unsigned int nNumVertices) = 0;
	virtual void DrawQuadNew(float x, float y, float w, float h, const float4& vColor, float fZValue, float tx, float ty, float wtx, float wty, bool bUseDefaultVS, ZRenderShader* pFragmentShader_, ZRenderShader* pVertexShader_, const ZRenderConstBufferRef ConstBufferF, const ZRenderConstBufferRef ConstBufferV) = 0;
	virtual void DrawPrimitives(ERenderPrimitiveTopology ePrimitiveTopology, const void* pVertices, unsigned int nNumPrimitives, unsigned int nVertexSize, const bool bAddToStats) = 0;
	virtual void BeginVertices(ERenderPrimitiveTopology ePrimitiveTopology, unsigned int nVertexCount, unsigned int nVertexStride, void** ppVertexData, const bool bAddToStats) = 0;
	virtual void EndVertices() = 0;
	virtual void Dispatch(unsigned int nThreadGroupsX, unsigned int nThreadGroupsY, unsigned int nThreadGroupsZ, ZRenderShader* pComputeShader, const ZRenderConstBufferRef ConstBufferC) = 0;
	virtual void SetVertexBuffers(unsigned int nStartSlot, unsigned int nNumBuffers, ZRenderVertexBuffer** pVertexBuffers, const unsigned char* pStrides) = 0;
	virtual void SetVertexBuffer(ZRenderVertexBuffer* pVertexBuffers, const unsigned char nStride) = 0;
	virtual void SetIndexBuffer(ZRenderIndexBuffer* pIndexBuffer, ERenderFormat eFormat, unsigned int nOffset) = 0;
	virtual void SetInputLayout(ZRenderInputLayout* pInputLayout) = 0;
	virtual void SetBlendState(ZRenderBlendState* pBlendState, float fBlendFactor) = 0;
	virtual void SetRasterizerState(ZRenderRasterizerState* pRasterizerState) = 0;
	virtual void SetDepthStencilState(ZRenderDepthStencilState* pDepthStencilState, unsigned int nStencilRef) = 0;
	virtual void GetRenderTargets3D(unsigned int nNumViews, ZRenderTargetView3D** ppRenderTargetViews3D) = 0;
	virtual void SetRenderTargets3D(unsigned int nNumViews, ZRenderTargetView3D** ppRenderTargetViews3D, unsigned int nClearFlags, const float4* pClearColors) = 0;
	virtual void GetRenderTargets(unsigned int nNumViews, ZRenderTargetView** ppRenderTargetViews, ZRenderDepthStencilView** ppDepthStencilView) = 0;
	virtual void SetRenderTargets(unsigned int nNumViews, ZRenderTargetView** ppRenderTargetViews, ZRenderDepthStencilView* pDepthStencilView, unsigned int nClearFlags, const float4* pClearColors, float fDepthValue, unsigned int nStencilValue) = 0;
	virtual void ClearRenderTargets(unsigned int nNumViews, unsigned int nClearFlags, const float4* pClearColors, float fDepthValue, unsigned int nStencilValue) = 0;
	virtual void ClearHiStencil(unsigned int nFlags, unsigned int nStencilValue) = 0;
	virtual void ResolveRTV(ZRenderTargetView* pDstRTV, ZRenderTargetView* pSrcRTV) = 0;
	virtual void ResolveDSV(ZRenderDepthStencilView* pDstDSV, ZRenderDepthStencilView* pSrcDSV) = 0;
	virtual void CopyResource(IRenderResource* pDstResource, IRenderResource* pSrcResource) = 0;
	virtual const SRenderDeviceCaps& GetDeviceCaps() const = 0;

	ID3D11Device* GetDirect3DDevice() const;
	ID3D11DeviceContext* GetImmediateContext() const;

private:
	PAD(0xE4);
	ZRenderSwapChain* m_pSwapChain; //0xE8
	PAD(0x60);
	ID3D11Device* m_pDirect3DDevice; //0x14C
	ID3D11DeviceContext* m_pDeviceContextImmediate; //0x150
	PAD(0x13C);
};

static_assert(sizeof(ZRenderDevice) == 0x290);
static_assert(alignof(ZRenderDevice) == 0x8);
