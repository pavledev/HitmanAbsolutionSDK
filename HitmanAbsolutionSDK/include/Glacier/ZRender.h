#pragma once

#include <d3d11.h>

#include "IComponentInterface.h"
#include "ZEntity.h"
#include "ZMath.h"
#include "ZEvent.h"
#include "TList.h"
#include "ZPresenceState.h"

template<typename T> class TRefCountPtr;
template<typename T> class TEnumerator;
class IRenderPrimitive;
class IRenderMaterialEntity;
enum ERenderGlowTypes;
struct SRenderDestinationDesc;
struct SRenderViewport;
enum ERenderPrimitiveTopology;
class ZRenderShader;
class ZRenderConstBufferRef;
class ZRenderVertexBuffer;
class ZRenderIndexBuffer;
class ZRenderInputLayout;
class ZRenderBlendState;
class ZRenderRasterizerState;
class ZRenderDepthStencilState;
class ZRenderTargetView3D;
struct SRenderDeviceCaps;
class ZRenderTexture3D;
struct SRenderTexture3DDesc;
struct SRenderSubResourceData;

enum ERenderDestinationType
{
    RENDER_DESTINATION_TYPE_SCREEN = 0,
    RENDER_DESTINATION_TYPE_OFFSCREEN = 1
};

enum ERenderFormat
{
    RENDER_FORMAT_NONE = 0,
    RENDER_FORMAT_R32G32B32A32_TYPELESS = 1,
    RENDER_FORMAT_R32G32B32A32_FLOAT = 2,
    RENDER_FORMAT_R32G32B32A32_UINT = 3,
    RENDER_FORMAT_R32G32B32A32_SINT = 4,
    RENDER_FORMAT_R32G32B32_TYPELESS = 5,
    RENDER_FORMAT_R32G32B32_FLOAT = 6,
    RENDER_FORMAT_R32G32B32_UINT = 7,
    RENDER_FORMAT_R32G32B32_SINT = 8,
    RENDER_FORMAT_R16G16B16A16_TYPELESS = 9,
    RENDER_FORMAT_R16G16B16A16_FLOAT = 10,
    RENDER_FORMAT_R16G16B16A16_UNORM = 11,
    RENDER_FORMAT_R16G16B16A16_UINT = 12,
    RENDER_FORMAT_R16G16B16A16_SNORM = 13,
    RENDER_FORMAT_R16G16B16A16_SINT = 14,
    RENDER_FORMAT_R32G32_TYPELESS = 15,
    RENDER_FORMAT_R32G32_FLOAT = 16,
    RENDER_FORMAT_R32G32_UINT = 17,
    RENDER_FORMAT_R32G32_SINT = 18,
    RENDER_FORMAT_R32G8X24_TYPELESS = 19,
    RENDER_FORMAT_D32_FLOAT_S8X24_UINT = 20,
    RENDER_FORMAT_R32_FLOAT_X8X24_TYPELESS = 21,
    RENDER_FORMAT_X32_TYPELESS_G8X24_UINT = 22,
    RENDER_FORMAT_R10G10B10A2_TYPELESS = 23,
    RENDER_FORMAT_R10G10B10A2_UNORM = 24,
    RENDER_FORMAT_R10G10B10A2_UINT = 25,
    RENDER_FORMAT_R11G11B10_FLOAT = 26,
    RENDER_FORMAT_R8G8B8A8_TYPELESS = 27,
    RENDER_FORMAT_R8G8B8A8_UNORM = 28,
    RENDER_FORMAT_R8G8B8A8_UNORM_SRGB = 29,
    RENDER_FORMAT_R8G8B8A8_UINT = 30,
    RENDER_FORMAT_R8G8B8A8_SNORM = 31,
    RENDER_FORMAT_R8G8B8A8_SINT = 32,
    RENDER_FORMAT_R16G16_TYPELESS = 33,
    RENDER_FORMAT_R16G16_FLOAT = 34,
    RENDER_FORMAT_R16G16_UNORM = 35,
    RENDER_FORMAT_R16G16_UINT = 36,
    RENDER_FORMAT_R16G16_SNORM = 37,
    RENDER_FORMAT_R16G16_SINT = 38,
    RENDER_FORMAT_R32_TYPELESS = 39,
    RENDER_FORMAT_D32_FLOAT = 40,
    RENDER_FORMAT_R32_FLOAT = 41,
    RENDER_FORMAT_R32_UINT = 42,
    RENDER_FORMAT_R32_SINT = 43,
    RENDER_FORMAT_R24G8_TYPELESS = 44,
    RENDER_FORMAT_D24_UNORM_S8_UINT = 45,
    RENDER_FORMAT_R24_UNORM_X8_TYPELESS = 46,
    RENDER_FORMAT_X24_TYPELESS_G8_UINT = 47,
    RENDER_FORMAT_R9G9B9E5_SHAREDEXP = 48,
    RENDER_FORMAT_R8G8_B8G8_UNORM = 49,
    RENDER_FORMAT_G8R8_G8B8_UNORM = 50,
    RENDER_FORMAT_R8G8_TYPELESS = 51,
    RENDER_FORMAT_R8G8_UNORM = 52,
    RENDER_FORMAT_R8G8_UINT = 53,
    RENDER_FORMAT_R8G8_SNORM = 54,
    RENDER_FORMAT_R8G8_SINT = 55,
    RENDER_FORMAT_R16_TYPELESS = 56,
    RENDER_FORMAT_R16_FLOAT = 57,
    RENDER_FORMAT_D16_UNORM = 58,
    RENDER_FORMAT_R16_UNORM = 59,
    RENDER_FORMAT_R16_UINT = 60,
    RENDER_FORMAT_R16_SNORM = 61,
    RENDER_FORMAT_R16_SINT = 62,
    RENDER_FORMAT_B5G6R5_UNORM = 63,
    RENDER_FORMAT_B5G5R5A1_UNORM = 64,
    RENDER_FORMAT_R8_TYPELESS = 65,
    RENDER_FORMAT_R8_UNORM = 66,
    RENDER_FORMAT_R8_UINT = 67,
    RENDER_FORMAT_R8_SNORM = 68,
    RENDER_FORMAT_R8_SINT = 69,
    RENDER_FORMAT_A8_UNORM = 70,
    RENDER_FORMAT_R1_UNORM = 71,
    RENDER_FORMAT_BC1_TYPELESS = 72,
    RENDER_FORMAT_BC1_UNORM = 73,
    RENDER_FORMAT_BC1_UNORM_SRGB = 74,
    RENDER_FORMAT_BC2_TYPELESS = 75,
    RENDER_FORMAT_BC2_UNORM = 76,
    RENDER_FORMAT_BC2_UNORM_SRGB = 77,
    RENDER_FORMAT_BC3_TYPELESS = 78,
    RENDER_FORMAT_BC3_UNORM = 79,
    RENDER_FORMAT_BC3_UNORM_SRGB = 80,
    RENDER_FORMAT_BC4_TYPELESS = 81,
    RENDER_FORMAT_BC4_UNORM = 82,
    RENDER_FORMAT_BC4_SNORM = 83,
    RENDER_FORMAT_BC5_TYPELESS = 84,
    RENDER_FORMAT_BC5_UNORM = 85,
    RENDER_FORMAT_BC5_SNORM = 86,
    RENDER_FORMAT_R16G16B16_FLOAT = 87,
    RENDER_FORMAT_INDEX_32 = 88,
    RENDER_FORMAT_INDEX_16 = 89,
    RENDER_FORMAT_LE_X2R10G10B10_UNORM = 90,
    RENDER_FORMAT_LE_X8R8G8B8_UNORM = 91,
    RENDER_FORMAT_X16Y16Z16_SNORM = 92
};

enum ERenderResourceMipInterpolation
{
    eRENDER_RESOURCE_MIP_INTERPOLATION_NONE = 0,
    eRENDER_RESOURCE_MIP_INTERPOLATION_POINT = 1,
    eRENDER_RESOURCE_MIP_INTERPOLATION_LINEAR = 2,
    eRENDER_RESOURCE_MIP_INTERPOLATION_COUNT = 3
};

enum ERenderResourcePriority
{
    eRENDER_RESOURCE_PRIORITY_NORMAL = 0,
    eRENDER_RESOURCE_PRIORITY_CRUCIAL = 1
};

enum ERenderResourceUsage
{
    RENDER_RESOURCE_USAGE_DEFAULT = 1,
    RENDER_RESOURCE_USAGE_IMMUTABLE = 2,
    RENDER_RESOURCE_USAGE_DYNAMIC = 3,
    RENDER_RESOURCE_USAGE_STAGING = 4
};

enum ERenderResourceType
{
    RENDER_RESOURCE_TYPE_TEXTURE2D = 1,
    RENDER_RESOURCE_TYPE_TEXTURE3D = 2
};

template<typename T, int U> class TRenderReferencedCountedImpl : public T
{
  public:
    virtual ~TRenderReferencedCountedImpl() = 0;
    virtual void AddRef() = 0;
    virtual uint32_t Release() = 0;

    int32_t m_ReferenceCount;
};

template<typename T, int U> class TRenderResourceImpl : public TRenderReferencedCountedImpl<T, U>
{
  public:
    virtual ~TRenderResourceImpl() = 0;
    virtual ERenderResourceType GetResourceType() const = 0;
};

class ICurveChanged : public IComponentInterface
{};

class IRenderDestinationEntity : public IComponentInterface
{
  public:
    virtual const ZEntityRef& GetSource() const = 0;
    virtual void IRenderDestinationEntity_Unk6() = 0;
    virtual void IRenderDestinationEntity_Unk7() const = 0;
    virtual void IRenderDestinationEntity_Unk8() const = 0;
    virtual void SetSource(const ZEntityRef& entityRef) = 0;
    virtual void IRenderDestinationEntity_Unk10(bool param1) = 0;
    virtual void IRenderDestinationEntity_Unk11() = 0;
};

class IRenderPostfilterParametersEntity : public IComponentInterface
{};

class IRenderPostfilterControllerEntity : public IComponentInterface
{
  public:
    virtual TEntityRef<IRenderPostfilterParametersEntity> GetCurrentParametersEntity() const = 0;
    virtual TEntityRef<IRenderPostfilterParametersEntity> GetParametersEntity() const = 0;
};

class IRenderRefCount
{
  public:
    virtual ~IRenderRefCount() = 0;
    virtual void AddRef() = 0;
    virtual uint32_t Release() = 0;
};

class IRenderResource : public IRenderRefCount
{
  public:
    virtual ~IRenderResource() = 0;
    virtual ERenderResourceType GetResourceType() const = 0;
};

struct SRenderDepthStencilViewDesc
{
    enum EViewDimension
    {
        VIEW_DIMENSION_TEXTURE2D = 1,
        VIEW_DIMENSION_TEXTURE2DMS = 2
    };

    struct SViewTexture2D
    {
        uint32_t nMipSlice;
    };

    ERenderFormat eFormat;
    EViewDimension eViewDimension;
    SViewTexture2D viewTexture2D;
};

struct SRenderDestinationDesc
{
    ZString m_sName;
    uint32_t m_nWidth;
    uint32_t m_nHeight;
    uint32_t m_nWindowHandle;
    uint32_t m_nAdapterID;
    uint32_t m_nMonitorID;
    uint32_t m_nRefreshRate;
    uint32_t m_nParentWindowHandle;
    bool m_bFullScreen;
    bool m_bExclusiveFullscreen;
    bool m_bStereoscopic;
    float m_fAspectRatio;
    float m_fPrimaryAspectRatio;
    uint32_t m_nMultiSampleCount;
    ERenderFormat m_eFormat;
    ERenderDestinationType m_eDestinationType;
    bool m_bProvideDepth : 1;
};

struct SRenderPostfilterParametersBase
{
    bool m_bEnabled;
};

struct SRenderPostfilterParametersColorCorrection : SRenderPostfilterParametersBase
{
    uint32_t m_Palette[7][256];
    float m_fNearRange;
    float m_fFarRange;
    uint32_t m_nVersion;
    bool m_bDepthRemapEnabled;
    bool m_bColorCorrectFar;
};

struct SRenderSampleDesc
{
    uint32_t nCount;
    uint32_t nQuality;
};

struct SRenderTargetViewDesc
{
    enum EViewDimension
    {
        VIEW_DIMENSION_TEXTURE2D = 1,
        VIEW_DIMENSION_TEXTURE2DMS = 2,
        VIEW_DIMENSION_TEXTURE2DARRAY = 3,
        VIEW_DIMENSION_TEXTURE3D = 4
    };

    struct SViewTexture2D
    {
        uint32_t nMipSlice;
    };

    struct SViewTexture2DArray
    {
        uint32_t nMipSlice;
        uint32_t nFirstArraySlice;
        uint32_t nArraySize;
    };

    struct SViewTexture3D
    {
        uint32_t nMipSlice;
        uint32_t nFirstWSlice;
        uint32_t nWSize;
    };

    ERenderFormat eFormat;
    EViewDimension eViewDimension;

    union
    {
        SViewTexture2D viewTexture2D;
        SViewTexture2DArray viewTexture2DArray;
        SViewTexture3D viewTexture3D;
    };
};

struct SRenderTexture2DDesc
{
    uint64_t nResourceMemoryScopeID;
    uint32_t nWidth;
    uint32_t nHeight;
    uint32_t nMipLevels;
    uint32_t nArraySize;
    ERenderFormat eFormat;
    SRenderSampleDesc sampleDesc;
    uint32_t nBindFlags;
    uint32_t nMiscFlags;
    uint32_t nCPUAccessFlags;
    ERenderResourceUsage eUsage;
    ERenderResourcePriority ePriority;
    ERenderResourceMipInterpolation eMipInterpolation;
};

class ZBoundedEntity;

class ZSpatialEntity : public ZEntityImpl
{
  public:
    enum ERoomBehaviour
    {
        ROOM_STATIC = 0,
        ROOM_DYNAMIC = 1
    };

    virtual void SetVisible(bool bVisible) = 0;
    virtual const ZBoundedEntity* QueryBoundedEntity() const = 0;
    virtual ZBoundedEntity* QueryBoundedEntity() = 0;
    virtual void SetPrivate(bool bPrivate) = 0;
    virtual bool IsPrivate() const = 0;
    virtual void SetFPSDrawMode(bool bFPSDrawMode) = 0;
    virtual void SetEditorSelected(bool bSelected) = 0;
    virtual bool GetEditorSelected() const = 0;
    virtual void SetEditorVisible(bool bVisible) = 0;
    virtual bool GetEditorVisible() const = 0;
    virtual void SetTransformParent(TEntityRef<ZSpatialEntity> Parent) = 0;
    virtual TEntityRef<ZSpatialEntity> GetTransformParentEntityPtr() const = 0;
    virtual ZSpatialEntity* GetScene() const = 0;
    virtual void CalculateBounds(float4& vMin_, float4& vMax_, const uint32_t nIncludeFlags, const uint32_t nExcludeFlags) = 0;
    virtual float4 GetLocalCenter() const = 0;
    virtual float4 GetLocalHalfSize() const = 0;
    virtual void OnObjectToParentChanged() = 0;
    virtual void OnTransformParentIDChanged() = 0;
    virtual void OnVisibleChanged(const bool& bWasVisible) = 0;
    virtual void OnSpatialChange() = 0;

    SMatrix GetObjectToWorldMatrix() const
    {
        SMatrix result;
        Functions::ZSpatialEntity_GetObjectToWorldMatrix->Call(this, result);

        return result;
    }

    void SetObjectToWorldMatrix(const SMatrix& p_ObjectToWorld)
    {
        Functions::ZSpatialEntity_SetObjectToWorldMatrix->Call(this, p_ObjectToWorld);
    }

    float4 GetWorldPosition() const
    {
        float4 result;
        Functions::ZSpatialEntity_GetWorldPosition->Call(this, result);

        return result;
    }

    void SetWorldPosition(const float4& p_WorldPosition)
    {
        Functions::ZSpatialEntity_SetWorldPosition->Call(this, p_WorldPosition);
    }

    SMatrix GetViewMatrix() const
    {
        return GetObjectToWorldMatrix().Inverse();
    }

    ZSpatialEntity* m_pTransformParent;
    ZSpatialEntity* m_pTransformChildren;
    ZSpatialEntity* m_pTransformNext;
    ZSpatialEntity* m_pTransformPrev;
    ZEntityRef m_transformParentID;
    SVector4 m_vObjectToParentRotation;
    SVector3 m_vObjectToParentTranslation;
    bool m_bVisible;
    bool m_bIsPrivate;
    bool m_bDisplayBounds : 1;
    bool m_bDisplayPivot : 1;
    bool m_bDisplayHelpers : 1;
    bool m_bDisplayHelperPrimitive : 1;
    bool m_bEditorSelected : 1;
    bool m_bEditorVisible : 1;
    bool m_bParentHidden : 1;
    bool m_bSceneChild : 1;
    bool m_bFPSDrawMode : 1;
    bool m_bNotifyChange : 1;
    bool m_bRoomDynamicAlways : 1;
    bool m_bRoomDynamic : 1;
    bool m_bParentDynamic : 1;
    bool m_bIsDynamic : 1;
    bool m_bIncludeInParentsBounds : 1;
    bool m_bUpdateBoundsPending : 1;
    ZEvent<ZEntityRef const&, SMatrix43 const&, ZEventNull, ZEventNull, ZEventNull>* m_pTransformChangeCallBackEvent;
};

static_assert(sizeof(ZSpatialEntity) == 0x40);
static_assert(alignof(ZSpatialEntity) == 0x4);

class ZBoundedEntity : public ZSpatialEntity
{
  public:
    virtual void UpdateBounds() = 0;

    PAD(0x18);
};

static_assert(sizeof(ZBoundedEntity) == 0x58);
static_assert(alignof(ZBoundedEntity) == 0x4);

class ZCompositeEntity : public ZSpatialEntity
{};

class ZRenderableEntity : public ZBoundedEntity
{
  public:
    virtual void OnVisibleOnScreenChanged(bool bVisibleOnScreen) = 0;

    PAD(0x8);
};

static_assert(sizeof(ZRenderableEntity) == 0x60);
static_assert(alignof(ZRenderableEntity) == 0x4);

class ZPrimitiveContainerEntity : public ZRenderableEntity
{
  public:
    virtual void ClearPrimitives() = 0;
    virtual void ClearDecalPrimitives() = 0;
    virtual bool IsPrimitiveAdded(const TRefCountPtr<IRenderPrimitive>& pPrimitive) const = 0;
    virtual void AddPrimitive(const TRefCountPtr<IRenderPrimitive>& pPrimitive) = 0;
    virtual void AddPrimitiveNoBoundsUpdate(const TRefCountPtr<IRenderPrimitive>& pPrimitive) = 0;
    virtual void RemovePrimitive(const TRefCountPtr<IRenderPrimitive>& pPrimitive) = 0;
    virtual void RemovePrimitiveNoBoundsUpdate(const TRefCountPtr<IRenderPrimitive>& pPrimitive) = 0;
    virtual void PrimitiveUpdated(const TRefCountPtr<IRenderPrimitive>& pPrimitive, bool bUpdateBounds) = 0;
    virtual TEnumerator<TRefCountPtr<IRenderPrimitive> const> GetPrimitiveEnumerator() const = 0;
    virtual TEnumerator<TRefCountPtr<IRenderPrimitive>> GetPrimitiveEnumerator() = 0;
    virtual unsigned int GetPrimitivesCount() const = 0;
    virtual unsigned int GetPrimitiveChangeCounter() const = 0;
    virtual bool GetCastShadows() const = 0;
    virtual float GetLODScale() const = 0;
    virtual float GetLODOffset() const = 0;
    virtual void SetCastShadows(bool bCastShadows) = 0;
    virtual void SetLODScale(float fLODScale) = 0;
    virtual void SetLODOffset(float fLODOffset) = 0;
    virtual void AttachMaterialModifier(IRenderMaterialEntity* pMaterialModifier) = 0;
    virtual void DetachMaterialModifier(IRenderMaterialEntity* pMaterialModifier) = 0;
    virtual TEnumerator<IRenderMaterialEntity* const> GetMaterialModifiers() const = 0;
    virtual TEnumerator<IRenderMaterialEntity*> GetMaterialModifiers() = 0;
    virtual void ClearMaterialModifiers() = 0;
    virtual void SetOverrideMaterial(IRenderMaterialEntity* pMaterial) = 0;
    virtual void ForceUpdateOverrideMaterial(IRenderMaterialEntity* pMaterial) = 0;
    virtual IRenderMaterialEntity* GetOverrideMaterial() const = 0;
    virtual const ERenderGlowTypes GetGlowType() const = 0;
    virtual void SetGlowType(const ERenderGlowTypes eGlowType) = 0;

    uint16_t m_nRuntimeDecal;
    TArray<TRefCountPtr<IRenderPrimitive>> m_Primitives;
    TList<IRenderMaterialEntity*> m_pMaterialModifiers;
    IRenderMaterialEntity* m_pOverrideMaterial;
    uint32_t m_nPrimitiveChangeCounter;
    float m_fLODScale;
    float m_fLODOffset;
    ERenderGlowTypes m_eGlowType;
    bool m_bCastShadows;
    bool m_bPCCastShadows;
    bool m_bIsPlayer;
};

class ZGeomEntity : public ZPrimitiveContainerEntity, public ZPresenceState
{
  public:
    virtual ZResourcePtr GetResourcePtr() const = 0;
    virtual ZEvent<ZEventNull, ZEventNull, ZEventNull, ZEventNull, ZEventNull>& GetResourceChangedEvent() const = 0;
    virtual ZResourcePtr ChangePrimitiveResourcePtr(const ZResourcePtr& resourcePtr) = 0;
    virtual TEnumerator<TRefCountPtr<IRenderPrimitive> const> GetResourcePrimitiveEnumerator() const = 0;
    virtual TEnumerator<TRefCountPtr<IRenderPrimitive>> GetResourcePrimitiveEnumerator() = 0;
    virtual uint32_t GetResourcePrimitivesCount() const = 0;
    virtual void HandlePrimitiveChanged(const ZResourcePtr& oldValue) = 0;

  private:
    ZResourcePtr m_pPrimitive;
    ZEvent<ZEventNull, ZEventNull, ZEventNull, ZEventNull, ZEventNull>* m_pResourceChanged;
    uint16_t m_nRoomID;
};

class ZLinkedEntity : public ZGeomEntity
{};

class ZRenderTexture2D : public TRenderResourceImpl<IRenderResource, 1>
{
  public:
    virtual ~ZRenderTexture2D() = 0;
    virtual void Allocate(uint32_t nLocation) = 0;
    virtual void Free() = 0;

    SRenderTexture2DDesc m_Description;
    ID3D11Texture2D* m_pTexture2D;
    PAD(0x14);
};

static_assert(sizeof(ZRenderTexture2D) == 0x60);
static_assert(alignof(ZRenderTexture2D) == 0x8);

class ZRenderTargetView
{
  public:
    virtual ~ZRenderTargetView() = 0;

    SRenderTargetViewDesc m_Description;
    ZRenderTexture2D* m_pTexture;
    ID3D11RenderTargetView* m_pRTV;
};

static_assert(sizeof(ZRenderTargetView) == 0x20);
static_assert(alignof(ZRenderTargetView) == 0x4);

class ZRenderDepthStencilView
{
  public:
    virtual ~ZRenderDepthStencilView() = default;

    SRenderDepthStencilViewDesc m_Description;
    ZRenderTexture2D* m_pTexture;
    ID3D11DepthStencilView* m_pDSV;
};

class ZRenderDeviceBase
{
  public:
    virtual ~ZRenderDeviceBase() = 0;
    virtual ZRenderTexture2D*
    CreateTexture2D(const SRenderTexture2DDesc* renderTexture2DDesc, const SRenderSubResourceData* renderSubResourceData) = 0;
    virtual ZRenderTexture3D*
    CreateTexture3D(const SRenderTexture3DDesc* renderTexture3DDesc, const SRenderSubResourceData* renderSubResourceData) = 0;
    virtual void SetGamma(float param1) = 0;
    virtual void Trim() = 0;
    virtual void PlayCommandBufferWait() = 0;
};

class ZRenderSwapChain
{
  public:
    virtual ~ZRenderSwapChain() = 0;
    virtual void SetDisableStereoscopic(bool bDisable) = 0;

    PAD(0x8);
    IDXGISwapChain* m_pSwapChain; // 0xC
    PAD(0xC);
    ZRenderTexture2D* m_pBackBufferTexture; // 0x1C
    ZRenderTargetView* m_pBackBufferRTV;    // 0x20
    PAD(0x10);
    SRenderDestinationDesc* m_pDescription; // 0x34
    PAD(0x1C);
};

static_assert(sizeof(ZRenderSwapChain) == 0x54);
static_assert(alignof(ZRenderSwapChain) == 0x4);

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
    virtual void DrawIndexed(uint32_t nStartIndex, uint32_t nNumIndices, int nBaseVertex, uint32_t nNumVertices) = 0;
    virtual void DrawIndexedInstanced(
        uint32_t nIndexCountPerInstance, uint32_t nInstanceCount, uint32_t nStartIndex, int nBaseVertex, uint32_t nStartInstanceLocation,
        uint32_t nNumVertices
    ) = 0;
    virtual void DrawQuadNew(
        float x, float y, float w, float h, const float4& vColor, float fZValue, float tx, float ty, float wtx, float wty, bool bUseDefaultVS,
        ZRenderShader* pFragmentShader_, ZRenderShader* pVertexShader_, const ZRenderConstBufferRef ConstBufferF,
        const ZRenderConstBufferRef ConstBufferV
    ) = 0;
    virtual void DrawPrimitives(
        ERenderPrimitiveTopology ePrimitiveTopology, const void* pVertices, uint32_t nNumPrimitives, uint32_t nVertexSize, const bool bAddToStats
    ) = 0;
    virtual void BeginVertices(
        ERenderPrimitiveTopology ePrimitiveTopology, uint32_t nVertexCount, uint32_t nVertexStride, void** ppVertexData, const bool bAddToStats
    ) = 0;
    virtual void EndVertices() = 0;
    virtual void Dispatch(
        uint32_t nThreadGroupsX, uint32_t nThreadGroupsY, uint32_t nThreadGroupsZ, ZRenderShader* pComputeShader,
        const ZRenderConstBufferRef ConstBufferC
    ) = 0;
    virtual void SetVertexBuffers(uint32_t nStartSlot, uint32_t nNumBuffers, ZRenderVertexBuffer** pVertexBuffers, const uint8_t* pStrides) = 0;
    virtual void SetVertexBuffer(ZRenderVertexBuffer* pVertexBuffers, const uint8_t nStride) = 0;
    virtual void SetIndexBuffer(ZRenderIndexBuffer* pIndexBuffer, ERenderFormat eFormat, uint32_t nOffset) = 0;
    virtual void SetInputLayout(ZRenderInputLayout* pInputLayout) = 0;
    virtual void SetBlendState(ZRenderBlendState* pBlendState, float fBlendFactor) = 0;
    virtual void SetRasterizerState(ZRenderRasterizerState* pRasterizerState) = 0;
    virtual void SetDepthStencilState(ZRenderDepthStencilState* pDepthStencilState, uint32_t nStencilRef) = 0;
    virtual void GetRenderTargets3D(uint32_t nNumViews, ZRenderTargetView3D** ppRenderTargetViews3D) = 0;
    virtual void
    SetRenderTargets3D(uint32_t nNumViews, ZRenderTargetView3D** ppRenderTargetViews3D, uint32_t nClearFlags, const float4* pClearColors) = 0;
    virtual void GetRenderTargets(uint32_t nNumViews, ZRenderTargetView** ppRenderTargetViews, ZRenderDepthStencilView** ppDepthStencilView) = 0;
    virtual void SetRenderTargets(
        uint32_t nNumViews, ZRenderTargetView** ppRenderTargetViews, ZRenderDepthStencilView* pDepthStencilView, uint32_t nClearFlags,
        const float4* pClearColors, float fDepthValue, uint32_t nStencilValue
    ) = 0;
    virtual void
    ClearRenderTargets(uint32_t nNumViews, uint32_t nClearFlags, const float4* pClearColors, float fDepthValue, uint32_t nStencilValue) = 0;
    virtual void ClearHiStencil(uint32_t nFlags, uint32_t nStencilValue) = 0;
    virtual void ResolveRTV(ZRenderTargetView* pDstRTV, ZRenderTargetView* pSrcRTV) = 0;
    virtual void ResolveDSV(ZRenderDepthStencilView* pDstDSV, ZRenderDepthStencilView* pSrcDSV) = 0;
    virtual void CopyResource(IRenderResource* pDstResource, IRenderResource* pSrcResource) = 0;
    virtual const SRenderDeviceCaps& GetDeviceCaps() const = 0;

    PAD(0xE4);
    ZRenderSwapChain* m_pSwapChain; // 0xE8
    PAD(0x10);
    ZRenderTargetView* m_pRenderTargetViews[8]; // 0xFC
    PAD(0x30);
    ID3D11Device* m_pDirect3DDevice;                // 0x14C
    ID3D11DeviceContext* m_pDeviceContextImmediate; // 0x150
    PAD(0x13C);
};

static_assert(sizeof(ZRenderDevice) == 0x290);
static_assert(alignof(ZRenderDevice) == 0x8);

class ZRenderGBuffer
{
  public:
    PAD(0x10);
    ZRenderDevice* m_pRenderDevice; // 0x10
    PAD(0x8);
    ZRenderDepthStencilView* m_pDepthDSV; // 0x1C
};

class ZRenderContext
{
  public:
    PAD(0x40);
    ZRenderDevice* m_pRenderDevice; // 0x40
    PAD(0x2C);
    ZRenderGBuffer* m_pGBuffer; // 0x70
};

class alignas(8) ZRenderManager : public IComponentInterface
{
  public:
    TEntityRef<IRenderDestinationEntity> GetGameRenderDestinationEntity()
    {
        TEntityRef<IRenderDestinationEntity> result;
        Functions::ZRenderManager_GetGameRenderDestinationEntity->Call(this, result);

        return result;
    }

    TEntityRef<IRenderDestinationEntity> GetActiveRenderDestinationEntity() const
    {
        TEntityRef<IRenderDestinationEntity> result;
        Functions::ZRenderManager_GetActiveRenderDestinationEntity->Call(this, result);

        return result;
    }

    PAD(0x8C);
    ZRenderDevice* m_pRenderDevice; // 0x90
    PAD(0x12C);
    ZRenderContext* m_pRenderContext; // 0x1C0
    PAD(0x84);
};

static_assert(sizeof(ZRenderManager) == 0x248);
static_assert(alignof(ZRenderManager) == 0x8);

class ZRenderPostfilterControllerEntity : public ZRenderableEntity, public IRenderPostfilterControllerEntity
{
  public:
    ZEntityRef m_PostfilterParametersEntity; // 0x64
    PAD(0x24);
    ZEntityRef m_CurrentPostfilterParametersEntity; // 0x8C
};

class ZRenderPostfilterParametersEntity : public ZEntityImpl, public IRenderPostfilterParametersEntity, public ICurveChanged
{};
