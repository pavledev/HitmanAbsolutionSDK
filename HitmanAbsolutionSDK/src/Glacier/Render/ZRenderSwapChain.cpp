#include "Glacier/Render/ZRenderSwapChain.h"

IDXGISwapChain* ZRenderSwapChain::GetSwapChain() const
{
	return m_pSwapChain;
}

ZRenderTexture2D* ZRenderSwapChain::GetBackBufferTexture() const
{
	return m_pBackBufferTexture;
}

ZRenderTargetView* ZRenderSwapChain::GetBackBufferRTV() const
{
	return m_pBackBufferRTV;
}
