#include <Glacier/Engine/ZEngineAppCommon.h>
#include <Glacier/ZLevelManager.h>

#include <Function.h>
#include <Global.h>

void ZEngineAppCommon::CreateFreeCameraAndControl()
{
	Function::CallMethod<ZEngineAppCommon*>(BaseAddress + 0x3FA840, this);
}

TEntityRef<ZFreeCameraControlEntity> ZEngineAppCommon::GetFreeCameraControl() const
{
	return m_pFreeCameraControl;
}

TEntityRef<ZCameraEntity> ZEngineAppCommon::GetFreeCamera() const
{
	return m_pFreeCamera;
}

TEntityRef<ZCameraEntity> ZEngineAppCommon::GetMainCamera() const
{
	return m_pMainCamera;
}

void ZEngineAppCommon::CopyMainCameraSettingsToFreeCamera()
{
	const ZHM5MainCamera* mainCamera = LevelManager->GetHitman().GetRawPointer()->GetMainCamera();
	ZCameraEntity* freeCamera = m_pFreeCamera.GetRawPointer();

	freeCamera->SetObjectToWorldMatrix(mainCamera->GetObjectToWorldMatrix());
	freeCamera->SetClearColor(mainCamera->GetClearColor());
	freeCamera->SetNearZ(mainCamera->GetNearZ());
	freeCamera->SetFarZ(mainCamera->GetFarZ());
	freeCamera->SetFovYDeg(mainCamera->GetFovYDeg());
	freeCamera->SetPostfilter(mainCamera->GetPostfilter());
}

const bool ZEngineAppCommon::GetIsGameInputActive() const
{
	return m_bIsGameInputActive;
}

void ZEngineAppCommon::SetIsGameInputActive(bool isGameInputActive)
{
	m_bIsGameInputActive = isGameInputActive;
}
