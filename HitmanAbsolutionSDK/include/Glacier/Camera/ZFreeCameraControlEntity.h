#pragma once

#include "../Entity/ZEntityImpl.h"
#include "../Entity/ZEntityRef.h"
#include "../Math/SMatrix.h"

#include <Common.h>

class ZCameraEntity;

class HitmanAbsolutionSDK_API ZFreeCameraControlEntity : public ZEntityImpl
{
public:
	bool IsActive();
	void SetActive(bool bActive);
	bool IsGameControlActive();
	void SetIsGameControlActive(bool isGameControlActive);
	const ZCameraEntity* GetControlledCameraEntity() const;
	ZCameraEntity* GetControlledCameraEntity();
	const float GetMoveSpeed() const;
	const float GetTurnSpeed() const;
	const float GetMoveX() const;
	const float GetMoveY() const;
	const float GetMoveZ() const;
	const bool GetResetRoll() const;
	const float GetDeltaPitch() const;
	const float GetDeltaRoll() const;
	const float GetDeltaYaw() const;
	const float GetInitialFov() const;
	const float GetFov() const;
	const float GetDeltaFov() const;
	const int GetControllerID() const;
	void SetMovementSpeedMultiplier(const float movementSpeedMultiplier);
	void SetMoveX(const float moveX);
	void SetMoveY(const float moveY);
	void SetMoveZ(const float moveZ);
	void SetMoveInWorldSpace(const bool moveInWorldSpace);
	void SetResetRoll(const bool resetRoll);
	void SetDeltaPitch(const float deltaPitch);
	void SetRoll(const float roll);
	void SetDeltaYaw(const float deltaYaw);
	void SetDeltaRoll(const float deltaRoll);
	void SetFov(const float fov);
	void SetDeltaFov(const float deltaFov);
	void SetControllerID(const int controllerID);
	float4 GetUpdatedCameraPosition(float fMoveX, float fMoveY, float fMoveZ, const SMatrix& mCurrentCameraToWorld);
	SMatrix GetUpdatedCameraRotation(float fDeltaRoll, float fDeltaPitch, float fDeltaYaw, const SMatrix& mCurrentCameraToWorld);

private:
	ZEntityRef m_cameraEntity;
	bool m_bActive;
	bool m_bIsGameControlActive;
	ZCameraEntity* m_pControlledCameraEntity;
	float m_fMoveSpeed;
	float m_fTurnSpeed;
	float m_fMovementSpeedMultiplier;
	float m_fRotationSpeedMultiplier;
	float m_fMoveX;
	float m_fMoveY;
	float m_fMoveZ;
	bool m_bMoveInWorldSpace;
	bool m_bResetRoll;
	float m_fPitch;
	float m_fDeltaPitch;
	float m_fRoll;
	float m_fDeltaRoll;
	float m_fDeltaYaw;
	float m_fInitialFov;
	float m_fFov;
	float m_fDeltaFov;
	int m_nControllerId;
};
