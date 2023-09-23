#include "Glacier/Render/ZRenderTargetView.h"

ZRenderTexture2D* ZRenderTargetView::GetTexture() const
{
	return m_pTexture;
}

void ZRenderTargetView::GetDesc(SRenderTargetViewDesc* pDescription)
{
	*pDescription = m_Description;
}

ID3D11RenderTargetView* ZRenderTargetView::GetRenderTargetView() const
{
	return m_pRTV;
}
