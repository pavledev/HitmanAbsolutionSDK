#include "Glacier/Render/ZRenderTexture2D.h"

const SRenderTexture2DDesc& ZRenderTexture2D::GetDesc() const
{
	return m_Description;
}

ID3D11Texture2D* ZRenderTexture2D::GetTexture2D() const
{
	return m_pTexture2D;
}
