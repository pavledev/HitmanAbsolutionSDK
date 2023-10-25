#pragma once

#include "../IComponentInterface.h"
#include "../ZHM5BaseCharacter.h"
#include "../Camera/ZHM5MainCamera.h"
#include "../Input/ZHM5InputControl.h"
#include "../Physics/ICharacterCollision.h"
#include "ZHM5FocusController.h"
#include "../Resource/ZRuntimeResourceID.h"
#include "ZHM5BaseInventory.h"

class ZHM5Action;
struct float4;
struct SMatrix;
enum eMorphemeRequests;
struct SGameUpdateEvent;

class HitmanAbsolutionSDK_API IFutureCameraState : public IComponentInterface
{
public:
	struct SCameraState;

	virtual ~IFutureCameraState() = default;
	virtual SCameraState GetFutureCameraState() = 0;
	virtual SCameraState GetCurrentCameraState() = 0;
	virtual void DisableCameraControl() = 0;
	virtual void EnableCameraControl() = 0;
};

class HitmanAbsolutionSDK_API IHM5ActionEntityListener : public IComponentInterface
{
public:
	virtual ~IHM5ActionEntityListener() = default;
	virtual bool GetActionAABB(const ZHM5Action* pAction, float4& vMin, float4& vMax) const = 0;
	virtual bool IsActionValid(ZHM5Action* action, const TEntityRef<ZHM5BaseCharacter>& entityRef) = 0;
	virtual bool DisplayFarFeedback(ZHM5Action* pAction, const TEntityRef<ZHM5BaseCharacter>& pOperator) = 0;
	virtual bool ActivateAction(ZHM5Action* pAction, const TEntityRef<ZHM5BaseCharacter>& pOperator) = 0;
};

class HitmanAbsolutionSDK_API ZHM5MorphemeNodeIds
{
private:
	unsigned int m_nFullBodySM;
	unsigned int m_nLocomotion;
	unsigned int m_nUpperBodyOverrideSM;
	unsigned int m_nReloadSM;
	unsigned int m_nEquipItemSM;
	unsigned int m_nClimbLedgeSM;
	unsigned int m_nLedgeDismountSM;
	unsigned int m_nLedgeDismountUp;
	unsigned int m_nLedgeDismountUpSneak;
	unsigned int m_nLedgeDismountDown280;
	unsigned int m_nLedgeDismountDown280Sneak;
	unsigned int m_nLedgeDismountDown320;
	unsigned int m_nLedgeDismountDown320Sneak;
	unsigned int m_nLedgeDismountDown410Sneak;
	unsigned int m_nLedgeJumpLeftIdle;
	unsigned int m_nLedgeJumpRightIdle;
	unsigned int m_nGuideMountSM;
	unsigned int m_nMountToLedgeFromStand;
	unsigned int m_nLedgeMainSM;
	unsigned int m_nLedgeHangIdle;
	unsigned int m_nLedgeMoveLeft;
	unsigned int m_nLedgeMoveRight;
	unsigned int m_nClimbUpToLedgeWalk;
	unsigned int m_nClimbDownToLedgeHang;
	unsigned int m_nDropDownToNextLedge;
	unsigned int m_nJumpUpToNextLedge;
	unsigned int m_nLedgeWalkIdle;
	unsigned int m_nLedgeWalkMoveRightSM;
	unsigned int m_nLedgeWalkMoveLeftSM;
	unsigned int m_nLedgeHangMoveRightSM;
	unsigned int m_nLedgeHangMoveLeftSM;
	unsigned int m_nLedgeMoveMountLeft;
	unsigned int m_nLedgeMoveMountRight;
	unsigned int m_nLedgeMountJumpUp;
	unsigned int m_nLedgeMountJumpUp280;
	unsigned int m_nLedgeStepOffLeft;
	unsigned int m_nLedgeStepOffRight;
	unsigned int m_nLedgeWalkJumpRightIdle;
	unsigned int m_nLedgeWalkJumpLeftIdle;
	unsigned int m_nLedgeClimbWindowLeft;
	unsigned int m_nLedgeClimbWindowRight;
	unsigned int m_nWindowPullVictimStandingRight;
	unsigned int m_nWindowPullVictimStandingRightBack;
	unsigned int m_nWindowPullVictimStandingLeft;
	unsigned int m_nWindowPullVictimStandingLeftBack;
	unsigned int m_nPullVictimFromLedgeRootSM;
	unsigned int m_nPullVictimFromLedgeFront;
	unsigned int m_nPullVictimFromLedgeLeft;
	unsigned int m_nPullVictimFromLedgeRight;
	unsigned int m_nPullVictimFromLedgeBehind;
	unsigned int m_nSBRootNode;
	unsigned int m_nSBAimGrips;
	unsigned int m_nSBRelaxedGrips;
	unsigned int m_nChangeClothesSM;
	unsigned int m_ChangeClothesLowSM;
	unsigned int m_nChangeClothesLowStand;
	unsigned int m_nChangeClothesLowSneak;
	unsigned int m_nChangeClothesHigh;
	unsigned int m_nEnterCoverSM;
	unsigned int m_nEnterHighCoverRightSM;
	unsigned int m_nEnterHighCoverRight;
	unsigned int m_nEnterHighCoverRightFromSBSM;
	unsigned int m_nEnterHighCoverRightFromSB;
	unsigned int m_nEnterLowCoverRightSM;
	unsigned int m_nEnterLowCoverRight;
	unsigned int m_nEnterHighCoverLeftSM;
	unsigned int m_nEnterHighCoverLeft;
	unsigned int m_nEnterHighCoverLeftFromSBSM;
	unsigned int m_nEnterHighCoverLeftFromSB;
	unsigned int m_nEnterLowCoverLeftSM;
	unsigned int m_nEnterLowCoverLeft;
	unsigned int m_nEnterHighCoverRightBackSM;
	unsigned int m_nEnterHighCoverRightBack;
	unsigned int m_nEnterLowCoverRightBackSM;
	unsigned int m_nEnterLowCoverRightBack;
	unsigned int m_nEnterHighCoverLeftBackSM;
	unsigned int m_nEnterHighCoverLeftBack;
	unsigned int m_nEnterLowCoverLeftBackSM;
	unsigned int m_nEnterLowCoverLeftBack;
	unsigned int m_nEnterLowCoverLeftFromSBSM;
	unsigned int m_nEnterLowCoverLeftFromSB;
	unsigned int m_nEnterLowCoverCentreLeftFromSBSM;
	unsigned int m_nEnterLowCoverCentreLeftFromSB;
	unsigned int m_nEnterLowCoverRightFromSBSM;
	unsigned int m_nEnterLowCoverRightFromSB;
	unsigned int m_nEnterLowCoverCentreRightFromSBSM;
	unsigned int m_nEnterLowCoverCentreRightFromSB;
	unsigned int m_nCoverRefSM;
	unsigned int m_nCoverAimSM;
	unsigned int m_nCoverMainSM;
	unsigned int m_nInCoverSM;
	unsigned int m_nCoverIdleSM;
	unsigned int m_nCoverStandIdle;
	unsigned int m_nCoverMoveSM;
	unsigned int m_nCoverMoving;
	unsigned int m_nCoverToCoverSM;
	unsigned int m_nCToC_Low100;
	unsigned int m_nCToC_Low200;
	unsigned int m_nCToC_Low300;
	unsigned int m_nCToC_High100;
	unsigned int m_nCToC_High200;
	unsigned int m_nCToC_High300;
	unsigned int m_nCoverExitToSB;
	unsigned int m_nCoverExitToSBLeftStand;
	unsigned int m_nCoverExitToSBRightStand;
	unsigned int m_nCoverExitToSBLeftLow;
	unsigned int m_nCoverExitToSBRightLow;
	unsigned int m_nCoverExitToSBLeftHigh;
	unsigned int m_nCoverExitToSBRightHigh;
	unsigned int m_nCoverSwitchCoverFacing;
	unsigned int m_nCoverSwitchCoverFacingLow;
	unsigned int m_nCoverSwitchCoverFacingHigh;
	unsigned int m_nCoverScaleExitRight;
	unsigned int m_nCoverScaleExitLeft;
	unsigned int m_nCoverEnterAimSM;
	unsigned int m_nCoverExitAimSM;
	unsigned int m_nCoverInAimSM;
	unsigned int m_nCoverCrouchRightEnterCrouchAim;
	unsigned int m_nCoverCrouchRighthEnterStandAim;
	unsigned int m_nCoverStandRightEnterStandAim;
	unsigned int m_nCoverCrouchLeftEnterCrouchAim;
	unsigned int m_nCoverCrouchLeftEnterStandAim;
	unsigned int m_nCoverStandLeftEnterStandAim;
	unsigned int m_nCoverCrouchRightExitCrouchAim;
	unsigned int m_nCoverCrouchRightExitStandAim;
	unsigned int m_nCoverStandRightExitStandAim;
	unsigned int m_nCoverCrouchLeftExitCrouchAim;
	unsigned int m_nCoverCrouchLeftExitStandAim;
	unsigned int m_nCoverStandLeftExitStandAim;
	unsigned int m_nLadderRef;
	unsigned int m_nLadderMountSM;
	unsigned int m_nLadderDismountSM;
	unsigned int m_nLadderMountBottom;
	unsigned int m_nLadderMountTopLeftLeg;
	unsigned int m_nLadderMountTopRightLeg;
	unsigned int m_nLadderDismountmountBottom;
	unsigned int m_nLadderDismountmountTopLeftLeg;
	unsigned int m_nLadderDismountmountTopRightLeg;
	unsigned int m_nLadderSM;
	unsigned int m_nLadderStandIdle;
	unsigned int m_nLadderStandIdleRight;
	unsigned int m_nLadderUpFast;
	unsigned int m_nLadderUpSlow;
	unsigned int m_nLadderDownFast;
	unsigned int m_nLadderDownSlow;
	unsigned int m_nLadderSlideDown;
	unsigned int m_nLadderSlideDownSM;
	unsigned int m_nSpecialKillSM;
	unsigned int m_nFiberWire01;
	unsigned int m_nFiberWireLongDist;
	unsigned int m_nFiberWireCloseDist;
	unsigned int m_nFiberWireLongDistToStand;
	unsigned int m_nFiberWireLongDistToSneak;
	unsigned int m_nFiberWireCloseDistToStand;
	unsigned int m_nFiberWireCloseDistToSneak;
	unsigned int m_nPushVictimThroughWindow;
	unsigned int m_nKickVictimOverLedge;
	unsigned int m_nDumpBodyInContainer;
	unsigned int m_nDumpSecondBodyInContainer;
	unsigned int m_nDumpBodyInLaundryChute;
	unsigned int m_nDumpBodyOverRail;
	unsigned int m_nDumpBodyOverLedge;
	unsigned int m_nDumpBodyInCloset;
	unsigned int m_nDumpSecondBodyInCloset;
	unsigned int m_nDragBodySM;
	unsigned int m_nClimbWindowToLedgeLeft;
	unsigned int m_nClimbWindowToLedgeRight;
	unsigned int m_nSneakPastWindowToLeft;
	unsigned int m_nSneakPastWindowToRight;
	unsigned int m_nClimbWindowToGroundSM;
	unsigned int m_nClimbWindowToGroundLeft;
	unsigned int m_nClimbWindowToGroundRight;
	unsigned int m_nClimbWindowToGroundFromLowCover;
	unsigned int m_nDeadSM;
	unsigned int m_nClosetRef;
	unsigned int m_nClosetNormal;
	unsigned int m_nClosetCake;
	unsigned int m_nClosetDumpster;
	unsigned int m_nClosetNormalEnter;
	unsigned int m_nClosetNormalInside;
	unsigned int m_nClosetNormalExit;
	unsigned int m_nClosetCakeEnter;
	unsigned int m_nClosetCakeInside;
	unsigned int m_nClosetCakeExit;
	unsigned int m_nClosetDumpsterEnter;
	unsigned int m_nClosetDumpsterInside;
	unsigned int m_nClosetDumpsterExit;
	unsigned int m_nDisableFuseBox;
	unsigned int m_nVentilatorShaftSM;
	unsigned int m_nVentilatorShaftEnterHigh;
	unsigned int m_nVentilatorShaftEnterLow;
	unsigned int m_nVentilatorShaftExitForwardHigh;
	unsigned int m_nVentilatorShaftExitForwardLow;
	unsigned int m_nVentilatorShaftExitBackwardsHigh;
	unsigned int m_nVentilatorShaftExitBackwardsLow;
	unsigned int m_nVentilatorShaftForward;
	unsigned int m_nVentilatorShaftBackward;
	unsigned int m_nVentilatorShaftIdle;
	unsigned int m_nWeaponRecoilSM;
	unsigned int m_nRecoilPistol90;
	unsigned int m_nRecoilShotgunLow;
	unsigned int m_nRecoilShotgunHigh;
	unsigned int m_nPickLockSM;
	unsigned int m_nStartPickLock_NormalLock;
	unsigned int m_nLoopPickLock_NormalLock;
	unsigned int m_nEndPickLock_NormalLock;
	unsigned int m_nStartPickLock_PushBarPadLock;
	unsigned int m_nLoopPickLock_PushBarPadLock;
	unsigned int m_nEndPickLock_PushBarPadLock;
	unsigned int m_nSafeZoneState;
	unsigned int m_nSafeZoneChildNetwork;
	unsigned int m_nDisguiseActiveActWeight;
	unsigned int m_nDisguiseActiveActChildNetwork;
	unsigned int m_nMovementWildCardTransitID;
	unsigned int m_nCloseCombatBoss;
	unsigned int m_nSurrenderAnimation;
	unsigned int m_nTossSM;
	unsigned int m_nStandThrowSM;
	unsigned int m_nStandNormalThrowSM;
	unsigned int m_nStandNormalThrow;
	unsigned int m_nStandNormalThrow90Right;
	unsigned int m_nCrouchNormalThrow;
	unsigned int m_nCrouchNormalThrow90Right;
	unsigned int m_nStandDeadlyThrowSM;
	unsigned int m_nStandDeadly135Left;
	unsigned int m_nStandDeadlyThrow;
	unsigned int m_nStandDeadly135Right;
	unsigned int m_nCrouchDeadly135Left;
	unsigned int m_nCrouchDeadlyThrow;
	unsigned int m_nCrouchDeadly135Right;
	unsigned int m_nStandDeadlyHeavy135Left;
	unsigned int m_nStandDeadlyHeavy;
	unsigned int m_nStandDeadlyHeavy135Right;
	unsigned int m_nCrouchDeadlyHeavy135Left;
	unsigned int m_nCrouchDeadlyHeavy;
	unsigned int m_nCrouchDeadlyHeavy135Right;
	unsigned int m_nCoverThrowSM;
	unsigned int m_nCoverNormalThrowSM;
	unsigned int m_nCoverNormalThrowHighSM;
	unsigned int m_nCoverNormalThrowHighLeft;
	unsigned int m_nCoverNormalThrowHighRight;
	unsigned int m_nCoverNormalThrowHighLeft180;
	unsigned int m_nCoverNormalThrowHighRight180;
	unsigned int m_nCoverNormalThrowLowSM;
	unsigned int m_nCoverNormalThrowLowLeft;
	unsigned int m_nCoverNormalThrowLowRight;
	unsigned int m_nCoverNormalThrowLowLeft180;
	unsigned int m_nCoverNormalThrowLowRight180;
	unsigned int m_nCoverDeadlyThrowSM;
	unsigned int m_nCoverDeadlyThrowHighSM;
	unsigned int m_nCoverDeadlyHeavyHighLeft;
	unsigned int m_nCoverDeadlyHighLeft180;
	unsigned int m_nCoverDeadlyHeavyHighRight;
	unsigned int m_nCoverDeadlyHighRight180;
	unsigned int m_nCoverDeadlyHeavyLowLeft;
	unsigned int m_nCoverDeadlyLowLeft180;
	unsigned int m_nCoverDeadlyHeavyLowRight;
	unsigned int m_nCoverDeadlyLowRight180;
	unsigned int m_nCoverDeadlyHeavyHighRight180;
	unsigned int m_nCoverDeadlyHeavyLowRight180;
	unsigned int m_nCoverDeadlyHeavyHighLeft180;
	unsigned int m_nCoverDeadlyHeavyLowLeft180;
	unsigned int m_nCoverDeadlyThrowHighLeft;
	unsigned int m_nCoverDeadlyThrowHighRight;
	unsigned int m_nCoverDeadlyThrowLowSM;
	unsigned int m_nCoverDeadlyThrowLowLeft;
	unsigned int m_nCoverDeadlyThrowLowRight;
	unsigned int m_nContextActionState;
	unsigned int m_nContextAction;
	unsigned int m_nInteractiveCutsequence;
	unsigned int m_nImpactBack;
	unsigned int m_nImpactFront;
	unsigned int m_nImpactLeft;
	unsigned int m_nImpactRight;
	unsigned int m_nLedgeAimFrontR;
	unsigned int m_nLedgeAimSideR;
	unsigned int m_nLedgeAimCrossR;
	unsigned int m_nLedgeAimCornerR;
	unsigned int m_nLedgeAimFrontL;
	unsigned int m_nLedgeAimSideL;
	unsigned int m_nLedgeAimCrossL;
	unsigned int m_nLedgeAimCornerL;
	unsigned int m_nLedgeAimWindowR;
	unsigned int m_nLedgeAimWindowL;
	unsigned int m_nHighAimPose;
	unsigned int m_nLowAimPose;
	unsigned int m_nHighAimPoseCover;
	unsigned int m_nLowAimPoseCover;
	bool m_bInitialized;
};

class HitmanAbsolutionSDK_API ZHitman5 : public ZHM5BaseCharacter, public IFutureCameraState, public ICharacterCollision, public IHM5ActionEntityListener, public ZHM5MorphemeNodeIds
{
public:
	ZHM5MainCamera* GetMainCamera() const;
	ZHM5InputControl* GetInputControl() const;
	ZHM5BaseInventory* GetBaseInventory() const;
	ZHM5FocusController* GetFocusController() const;
	void EquipOutfitResource(ZRuntimeResourceID ridNewOutfitLibrary, const TEntityRef<IHM5Outfit> Outfit, bool bPlaceClothBundle, bool bForce);

private:
	PAD(0x3B0);
	ZHM5InputControl* m_pInputControl; //0xA30
	PAD(0x10);
	ZHM5BaseInventory* m_pBaseInventory; //0xA44
	PAD(0x10);
	ZHM5FocusController* m_pFocusController; //0xA58
	PAD(0x238);
	TEntityRef<ZHM5MainCamera> m_rMainCamera; //0xC94
	PAD(0x84);
};

static_assert(sizeof(ZHitman5) == 0xD20);
static_assert(alignof(ZHitman5) == 0x10);
