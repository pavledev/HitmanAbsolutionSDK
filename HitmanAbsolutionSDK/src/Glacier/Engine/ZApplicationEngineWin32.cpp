#include <Glacier/Engine/ZApplicationEngineWin32.h>
#include <Glacier/ZLevelManager.h>
#include <Glacier/Render/ZRenderManager.h>

#include "Global.h"
#include "Function.h"

ZApplicationEngineWin32* ZApplicationEngineWin32::GetInstance()
{
	return *m_pInstance;
}

void ZApplicationEngineWin32::SetInstance(ZApplicationEngineWin32** instance)
{
	m_pInstance = instance;
}

ZEngineAppCommon& ZApplicationEngineWin32::GetEngineAppCommon()
{
	return m_common;
}

HWND ZApplicationEngineWin32::GetHWND()
{
	return m_hWnd;
}

HICON ZApplicationEngineWin32::GetDefaultCursor()
{
    return m_hDefaultCursor;
}

void ZApplicationEngineWin32::SetShowingCursor(const bool showingCursor)
{
    m_bShowingCursor = showingCursor;
}

ZCameraEntity* ZApplicationEngineWin32::GetActiveCamera()
{
    ZFreeCameraControlEntity* freeCameraControlEntity = (*m_pInstance)->m_common.GetFreeCameraControl().GetRawPointer();

    if (freeCameraControlEntity)
    {
        ZCameraEntity* freeCamera = (*m_pInstance)->m_common.GetFreeCamera().GetRawPointer();

        if (freeCameraControlEntity->IsActive())
        {
            return freeCamera;
        }

        TEntityRef<IRenderDestinationEntity> renderDestinationEntity = RenderManager->GetGameRenderDestinationEntity();
        TEntityRef<ZCameraEntity> currentCamera = renderDestinationEntity.GetRawPointer()->GetSource();

        if (currentCamera.GetRawPointer() == freeCamera)
        {
            return freeCamera;
        }
    }

    ZHitman5* hitman = LevelManager->GetHitman().GetRawPointer();

    return hitman->GetMainCamera();
}
