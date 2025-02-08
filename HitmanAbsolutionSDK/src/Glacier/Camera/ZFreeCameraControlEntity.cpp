#include <Glacier/Camera/ZFreeCameraControlEntity.h>

#include <Function.h>
#include <Global.h>

bool ZFreeCameraControlEntity::IsActive()
{
	return m_bActive;
}

void ZFreeCameraControlEntity::SetActive(bool bActive)
{
	Function::CallMethod<ZFreeCameraControlEntity*, bool>(BaseAddress + 0x12A9E0, this, bActive);
}

bool ZFreeCameraControlEntity::IsGameControlActive()
{
	return m_bIsGameControlActive;
}

void ZFreeCameraControlEntity::SetIsGameControlActive(bool isGameControlActive)
{
	m_bIsGameControlActive = isGameControlActive;
}

const ZCameraEntity* ZFreeCameraControlEntity::GetControlledCameraEntity() const
{
	return m_pControlledCameraEntity;
}

ZCameraEntity* ZFreeCameraControlEntity::GetControlledCameraEntity()
{
	return m_pControlledCameraEntity;
}

const float ZFreeCameraControlEntity::GetMoveSpeed() const
{
	return m_fMoveSpeed;
}

const float ZFreeCameraControlEntity::GetTurnSpeed() const
{
	return m_fTurnSpeed;
}

const float ZFreeCameraControlEntity::GetMoveX() const
{
	return m_fMoveX;
}

const float ZFreeCameraControlEntity::GetMoveY() const
{
	return m_fMoveY;
}

const float ZFreeCameraControlEntity::GetMoveZ() const
{
	return m_fMoveZ;
}

const bool ZFreeCameraControlEntity::GetResetRoll() const
{
	return m_bResetRoll;
}

const float ZFreeCameraControlEntity::GetDeltaPitch() const
{
	return m_fDeltaPitch;
}

const float ZFreeCameraControlEntity::GetDeltaRoll() const
{
	return m_fDeltaRoll;
}

const float ZFreeCameraControlEntity::GetDeltaYaw() const
{
	return m_fDeltaYaw;
}

const float ZFreeCameraControlEntity::GetInitialFov() const
{
	return m_fInitialFov;
}

const float ZFreeCameraControlEntity::GetFov() const
{
	return m_fFov;
}

const float ZFreeCameraControlEntity::GetDeltaFov() const
{
	return m_fDeltaFov;
}

const int ZFreeCameraControlEntity::GetControllerID() const
{
	return m_nControllerId;
}

void ZFreeCameraControlEntity::SetMovementSpeedMultiplier(const float movementSpeedMultiplier)
{
	m_fMovementSpeedMultiplier = movementSpeedMultiplier;
}

void ZFreeCameraControlEntity::SetMoveX(const float moveX)
{
	m_fMoveX = moveX;
}

void ZFreeCameraControlEntity::SetMoveY(const float moveY)
{
	m_fMoveY = moveY;
}

void ZFreeCameraControlEntity::SetMoveZ(const float moveZ)
{
	m_fMoveZ = moveZ;
}

void ZFreeCameraControlEntity::SetMoveInWorldSpace(const bool moveInWorldSpace)
{
	m_bMoveInWorldSpace = moveInWorldSpace;
}

void ZFreeCameraControlEntity::SetResetRoll(const bool resetRoll)
{
	m_bResetRoll = resetRoll;
}

void ZFreeCameraControlEntity::SetDeltaPitch(const float deltaPitch)
{
	m_fDeltaPitch = deltaPitch;
}

void ZFreeCameraControlEntity::SetRoll(const float roll)
{
	m_fRoll = roll;
}

void ZFreeCameraControlEntity::SetDeltaYaw(const float deltaYaw)
{
	m_fDeltaYaw = deltaYaw;
}

void ZFreeCameraControlEntity::SetDeltaRoll(const float deltaRoll)
{
	m_fDeltaRoll = deltaRoll;
}

void ZFreeCameraControlEntity::SetFov(const float fov)
{
	m_fFov = fov;
}

void ZFreeCameraControlEntity::SetDeltaFov(const float deltaFov)
{
	m_fDeltaFov = deltaFov;
}

void ZFreeCameraControlEntity::SetControllerID(const int controllerID)
{
	m_nControllerId = controllerID;
}

float4 ZFreeCameraControlEntity::GetUpdatedCameraPosition(float fMoveX, float fMoveY, float fMoveZ, const SMatrix& mCurrentCameraToWorld)
{
	return Function::CallRVOMethodAndReturn<float4, ZFreeCameraControlEntity*, float, float, float, const SMatrix&>(BaseAddress + 0x431820, this, fMoveX, fMoveY, fMoveZ, mCurrentCameraToWorld);
}

SMatrix ZFreeCameraControlEntity::GetUpdatedCameraRotation(float fDeltaRoll, float fDeltaPitch, float fDeltaYaw, const SMatrix& mCurrentCameraToWorld)
{
	return Function::CallRVOMethodAndReturn<SMatrix, ZFreeCameraControlEntity*, float, float, float, const SMatrix&>(BaseAddress + 0x2E63B0, this, fDeltaRoll, fDeltaPitch, fDeltaYaw, mCurrentCameraToWorld);
}
