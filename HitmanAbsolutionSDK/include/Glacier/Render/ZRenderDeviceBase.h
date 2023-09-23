#pragma once

class ZRenderTexture2D;
struct SRenderTexture2DDesc;
struct SRenderSubResourceData;
class ZRenderTexture3D;
struct SRenderTexture3DDesc;

class ZRenderDeviceBase
{
public:
	virtual ~ZRenderDeviceBase() = 0;
	virtual ZRenderTexture2D* CreateTexture2D(const SRenderTexture2DDesc* renderTexture2DDesc, const SRenderSubResourceData* renderSubResourceData) = 0;
	virtual ZRenderTexture3D* CreateTexture3D(const SRenderTexture3DDesc* renderTexture3DDesc, const SRenderSubResourceData* renderSubResourceData) = 0;
	virtual void SetGamma(float param1) = 0;
	virtual void Trim() = 0;
	virtual void PlayCommandBufferWait() = 0;
};
