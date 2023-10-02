#include "Glacier/Render/ZRenderManager.h"

#include "Function.h"
#include "Global.h"

ZRenderDevice* ZRenderManager::GetRenderDevice() const
{
	return m_pRenderDevice;
}

TEntityRef<IRenderDestinationEntity> ZRenderManager::GetGameRenderDestinationEntity()
{
	return Function::CallRVOMethodAndReturn<TEntityRef<IRenderDestinationEntity>, ZRenderManager*>(BaseAddress + 0x315BE0, this);
}

TEntityRef<IRenderDestinationEntity> ZRenderManager::GetActiveRenderDestinationEntity() const
{
	return Function::CallRVOMethodAndReturn<TEntityRef<IRenderDestinationEntity>, const ZRenderManager*>(BaseAddress + 0x36800, this);
}
