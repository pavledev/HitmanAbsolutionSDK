#pragma once

#include "IComponentInterface.h"
#include "ZHM5BaseCharacter.h"
#include "ZCamera.h"
#include "ZInput.h"
#include "ZResource.h"
#include "ZPhysics.h"
#include "TFixedArray.h"

#include <Functions.h>

class ZHM5Action;
struct float4;
struct SMatrix;
enum eMorphemeRequests;
struct SGameUpdateEvent;

class IFutureCameraState : public IComponentInterface
{
  public:
    struct SCameraState;

    virtual ~IFutureCameraState() = default;
    virtual SCameraState GetFutureCameraState() = 0;
    virtual SCameraState GetCurrentCameraState() = 0;
    virtual void DisableCameraControl() = 0;
    virtual void EnableCameraControl() = 0;
};

class IHM5ActionEntityListener : public IComponentInterface
{
  public:
    virtual ~IHM5ActionEntityListener() = default;
    virtual bool GetActionAABB(const ZHM5Action* pAction, float4& vMin, float4& vMax) const = 0;
    virtual bool IsActionValid(ZHM5Action* action, const TEntityRef<ZHM5BaseCharacter>& entityRef) = 0;
    virtual bool DisplayFarFeedback(ZHM5Action* pAction, const TEntityRef<ZHM5BaseCharacter>& pOperator) = 0;
    virtual bool ActivateAction(ZHM5Action* pAction, const TEntityRef<ZHM5BaseCharacter>& pOperator) = 0;
};

class ZHM5MorphemeNodeIds
{
  public:
    uint32_t m_nFullBodySM;
    uint32_t m_nLocomotion;
    uint32_t m_nUpperBodyOverrideSM;
    uint32_t m_nReloadSM;
    uint32_t m_nEquipItemSM;
    uint32_t m_nClimbLedgeSM;
    uint32_t m_nLedgeDismountSM;
    uint32_t m_nLedgeDismountUp;
    uint32_t m_nLedgeDismountUpSneak;
    uint32_t m_nLedgeDismountDown280;
    uint32_t m_nLedgeDismountDown280Sneak;
    uint32_t m_nLedgeDismountDown320;
    uint32_t m_nLedgeDismountDown320Sneak;
    uint32_t m_nLedgeDismountDown410Sneak;
    uint32_t m_nLedgeJumpLeftIdle;
    uint32_t m_nLedgeJumpRightIdle;
    uint32_t m_nGuideMountSM;
    uint32_t m_nMountToLedgeFromStand;
    uint32_t m_nLedgeMainSM;
    uint32_t m_nLedgeHangIdle;
    uint32_t m_nLedgeMoveLeft;
    uint32_t m_nLedgeMoveRight;
    uint32_t m_nClimbUpToLedgeWalk;
    uint32_t m_nClimbDownToLedgeHang;
    uint32_t m_nDropDownToNextLedge;
    uint32_t m_nJumpUpToNextLedge;
    uint32_t m_nLedgeWalkIdle;
    uint32_t m_nLedgeWalkMoveRightSM;
    uint32_t m_nLedgeWalkMoveLeftSM;
    uint32_t m_nLedgeHangMoveRightSM;
    uint32_t m_nLedgeHangMoveLeftSM;
    uint32_t m_nLedgeMoveMountLeft;
    uint32_t m_nLedgeMoveMountRight;
    uint32_t m_nLedgeMountJumpUp;
    uint32_t m_nLedgeMountJumpUp280;
    uint32_t m_nLedgeStepOffLeft;
    uint32_t m_nLedgeStepOffRight;
    uint32_t m_nLedgeWalkJumpRightIdle;
    uint32_t m_nLedgeWalkJumpLeftIdle;
    uint32_t m_nLedgeClimbWindowLeft;
    uint32_t m_nLedgeClimbWindowRight;
    uint32_t m_nWindowPullVictimStandingRight;
    uint32_t m_nWindowPullVictimStandingRightBack;
    uint32_t m_nWindowPullVictimStandingLeft;
    uint32_t m_nWindowPullVictimStandingLeftBack;
    uint32_t m_nPullVictimFromLedgeRootSM;
    uint32_t m_nPullVictimFromLedgeFront;
    uint32_t m_nPullVictimFromLedgeLeft;
    uint32_t m_nPullVictimFromLedgeRight;
    uint32_t m_nPullVictimFromLedgeBehind;
    uint32_t m_nSBRootNode;
    uint32_t m_nSBAimGrips;
    uint32_t m_nSBRelaxedGrips;
    uint32_t m_nChangeClothesSM;
    uint32_t m_ChangeClothesLowSM;
    uint32_t m_nChangeClothesLowStand;
    uint32_t m_nChangeClothesLowSneak;
    uint32_t m_nChangeClothesHigh;
    uint32_t m_nEnterCoverSM;
    uint32_t m_nEnterHighCoverRightSM;
    uint32_t m_nEnterHighCoverRight;
    uint32_t m_nEnterHighCoverRightFromSBSM;
    uint32_t m_nEnterHighCoverRightFromSB;
    uint32_t m_nEnterLowCoverRightSM;
    uint32_t m_nEnterLowCoverRight;
    uint32_t m_nEnterHighCoverLeftSM;
    uint32_t m_nEnterHighCoverLeft;
    uint32_t m_nEnterHighCoverLeftFromSBSM;
    uint32_t m_nEnterHighCoverLeftFromSB;
    uint32_t m_nEnterLowCoverLeftSM;
    uint32_t m_nEnterLowCoverLeft;
    uint32_t m_nEnterHighCoverRightBackSM;
    uint32_t m_nEnterHighCoverRightBack;
    uint32_t m_nEnterLowCoverRightBackSM;
    uint32_t m_nEnterLowCoverRightBack;
    uint32_t m_nEnterHighCoverLeftBackSM;
    uint32_t m_nEnterHighCoverLeftBack;
    uint32_t m_nEnterLowCoverLeftBackSM;
    uint32_t m_nEnterLowCoverLeftBack;
    uint32_t m_nEnterLowCoverLeftFromSBSM;
    uint32_t m_nEnterLowCoverLeftFromSB;
    uint32_t m_nEnterLowCoverCentreLeftFromSBSM;
    uint32_t m_nEnterLowCoverCentreLeftFromSB;
    uint32_t m_nEnterLowCoverRightFromSBSM;
    uint32_t m_nEnterLowCoverRightFromSB;
    uint32_t m_nEnterLowCoverCentreRightFromSBSM;
    uint32_t m_nEnterLowCoverCentreRightFromSB;
    uint32_t m_nCoverRefSM;
    uint32_t m_nCoverAimSM;
    uint32_t m_nCoverMainSM;
    uint32_t m_nInCoverSM;
    uint32_t m_nCoverIdleSM;
    uint32_t m_nCoverStandIdle;
    uint32_t m_nCoverMoveSM;
    uint32_t m_nCoverMoving;
    uint32_t m_nCoverToCoverSM;
    uint32_t m_nCToC_Low100;
    uint32_t m_nCToC_Low200;
    uint32_t m_nCToC_Low300;
    uint32_t m_nCToC_High100;
    uint32_t m_nCToC_High200;
    uint32_t m_nCToC_High300;
    uint32_t m_nCoverExitToSB;
    uint32_t m_nCoverExitToSBLeftStand;
    uint32_t m_nCoverExitToSBRightStand;
    uint32_t m_nCoverExitToSBLeftLow;
    uint32_t m_nCoverExitToSBRightLow;
    uint32_t m_nCoverExitToSBLeftHigh;
    uint32_t m_nCoverExitToSBRightHigh;
    uint32_t m_nCoverSwitchCoverFacing;
    uint32_t m_nCoverSwitchCoverFacingLow;
    uint32_t m_nCoverSwitchCoverFacingHigh;
    uint32_t m_nCoverScaleExitRight;
    uint32_t m_nCoverScaleExitLeft;
    uint32_t m_nCoverEnterAimSM;
    uint32_t m_nCoverExitAimSM;
    uint32_t m_nCoverInAimSM;
    uint32_t m_nCoverCrouchRightEnterCrouchAim;
    uint32_t m_nCoverCrouchRighthEnterStandAim;
    uint32_t m_nCoverStandRightEnterStandAim;
    uint32_t m_nCoverCrouchLeftEnterCrouchAim;
    uint32_t m_nCoverCrouchLeftEnterStandAim;
    uint32_t m_nCoverStandLeftEnterStandAim;
    uint32_t m_nCoverCrouchRightExitCrouchAim;
    uint32_t m_nCoverCrouchRightExitStandAim;
    uint32_t m_nCoverStandRightExitStandAim;
    uint32_t m_nCoverCrouchLeftExitCrouchAim;
    uint32_t m_nCoverCrouchLeftExitStandAim;
    uint32_t m_nCoverStandLeftExitStandAim;
    uint32_t m_nLadderRef;
    uint32_t m_nLadderMountSM;
    uint32_t m_nLadderDismountSM;
    uint32_t m_nLadderMountBottom;
    uint32_t m_nLadderMountTopLeftLeg;
    uint32_t m_nLadderMountTopRightLeg;
    uint32_t m_nLadderDismountmountBottom;
    uint32_t m_nLadderDismountmountTopLeftLeg;
    uint32_t m_nLadderDismountmountTopRightLeg;
    uint32_t m_nLadderSM;
    uint32_t m_nLadderStandIdle;
    uint32_t m_nLadderStandIdleRight;
    uint32_t m_nLadderUpFast;
    uint32_t m_nLadderUpSlow;
    uint32_t m_nLadderDownFast;
    uint32_t m_nLadderDownSlow;
    uint32_t m_nLadderSlideDown;
    uint32_t m_nLadderSlideDownSM;
    uint32_t m_nSpecialKillSM;
    uint32_t m_nFiberWire01;
    uint32_t m_nFiberWireLongDist;
    uint32_t m_nFiberWireCloseDist;
    uint32_t m_nFiberWireLongDistToStand;
    uint32_t m_nFiberWireLongDistToSneak;
    uint32_t m_nFiberWireCloseDistToStand;
    uint32_t m_nFiberWireCloseDistToSneak;
    uint32_t m_nPushVictimThroughWindow;
    uint32_t m_nKickVictimOverLedge;
    uint32_t m_nDumpBodyInContainer;
    uint32_t m_nDumpSecondBodyInContainer;
    uint32_t m_nDumpBodyInLaundryChute;
    uint32_t m_nDumpBodyOverRail;
    uint32_t m_nDumpBodyOverLedge;
    uint32_t m_nDumpBodyInCloset;
    uint32_t m_nDumpSecondBodyInCloset;
    uint32_t m_nDragBodySM;
    uint32_t m_nClimbWindowToLedgeLeft;
    uint32_t m_nClimbWindowToLedgeRight;
    uint32_t m_nSneakPastWindowToLeft;
    uint32_t m_nSneakPastWindowToRight;
    uint32_t m_nClimbWindowToGroundSM;
    uint32_t m_nClimbWindowToGroundLeft;
    uint32_t m_nClimbWindowToGroundRight;
    uint32_t m_nClimbWindowToGroundFromLowCover;
    uint32_t m_nDeadSM;
    uint32_t m_nClosetRef;
    uint32_t m_nClosetNormal;
    uint32_t m_nClosetCake;
    uint32_t m_nClosetDumpster;
    uint32_t m_nClosetNormalEnter;
    uint32_t m_nClosetNormalInside;
    uint32_t m_nClosetNormalExit;
    uint32_t m_nClosetCakeEnter;
    uint32_t m_nClosetCakeInside;
    uint32_t m_nClosetCakeExit;
    uint32_t m_nClosetDumpsterEnter;
    uint32_t m_nClosetDumpsterInside;
    uint32_t m_nClosetDumpsterExit;
    uint32_t m_nDisableFuseBox;
    uint32_t m_nVentilatorShaftSM;
    uint32_t m_nVentilatorShaftEnterHigh;
    uint32_t m_nVentilatorShaftEnterLow;
    uint32_t m_nVentilatorShaftExitForwardHigh;
    uint32_t m_nVentilatorShaftExitForwardLow;
    uint32_t m_nVentilatorShaftExitBackwardsHigh;
    uint32_t m_nVentilatorShaftExitBackwardsLow;
    uint32_t m_nVentilatorShaftForward;
    uint32_t m_nVentilatorShaftBackward;
    uint32_t m_nVentilatorShaftIdle;
    uint32_t m_nWeaponRecoilSM;
    uint32_t m_nRecoilPistol90;
    uint32_t m_nRecoilShotgunLow;
    uint32_t m_nRecoilShotgunHigh;
    uint32_t m_nPickLockSM;
    uint32_t m_nStartPickLock_NormalLock;
    uint32_t m_nLoopPickLock_NormalLock;
    uint32_t m_nEndPickLock_NormalLock;
    uint32_t m_nStartPickLock_PushBarPadLock;
    uint32_t m_nLoopPickLock_PushBarPadLock;
    uint32_t m_nEndPickLock_PushBarPadLock;
    uint32_t m_nSafeZoneState;
    uint32_t m_nSafeZoneChildNetwork;
    uint32_t m_nDisguiseActiveActWeight;
    uint32_t m_nDisguiseActiveActChildNetwork;
    uint32_t m_nMovementWildCardTransitID;
    uint32_t m_nCloseCombatBoss;
    uint32_t m_nSurrenderAnimation;
    uint32_t m_nTossSM;
    uint32_t m_nStandThrowSM;
    uint32_t m_nStandNormalThrowSM;
    uint32_t m_nStandNormalThrow;
    uint32_t m_nStandNormalThrow90Right;
    uint32_t m_nCrouchNormalThrow;
    uint32_t m_nCrouchNormalThrow90Right;
    uint32_t m_nStandDeadlyThrowSM;
    uint32_t m_nStandDeadly135Left;
    uint32_t m_nStandDeadlyThrow;
    uint32_t m_nStandDeadly135Right;
    uint32_t m_nCrouchDeadly135Left;
    uint32_t m_nCrouchDeadlyThrow;
    uint32_t m_nCrouchDeadly135Right;
    uint32_t m_nStandDeadlyHeavy135Left;
    uint32_t m_nStandDeadlyHeavy;
    uint32_t m_nStandDeadlyHeavy135Right;
    uint32_t m_nCrouchDeadlyHeavy135Left;
    uint32_t m_nCrouchDeadlyHeavy;
    uint32_t m_nCrouchDeadlyHeavy135Right;
    uint32_t m_nCoverThrowSM;
    uint32_t m_nCoverNormalThrowSM;
    uint32_t m_nCoverNormalThrowHighSM;
    uint32_t m_nCoverNormalThrowHighLeft;
    uint32_t m_nCoverNormalThrowHighRight;
    uint32_t m_nCoverNormalThrowHighLeft180;
    uint32_t m_nCoverNormalThrowHighRight180;
    uint32_t m_nCoverNormalThrowLowSM;
    uint32_t m_nCoverNormalThrowLowLeft;
    uint32_t m_nCoverNormalThrowLowRight;
    uint32_t m_nCoverNormalThrowLowLeft180;
    uint32_t m_nCoverNormalThrowLowRight180;
    uint32_t m_nCoverDeadlyThrowSM;
    uint32_t m_nCoverDeadlyThrowHighSM;
    uint32_t m_nCoverDeadlyHeavyHighLeft;
    uint32_t m_nCoverDeadlyHighLeft180;
    uint32_t m_nCoverDeadlyHeavyHighRight;
    uint32_t m_nCoverDeadlyHighRight180;
    uint32_t m_nCoverDeadlyHeavyLowLeft;
    uint32_t m_nCoverDeadlyLowLeft180;
    uint32_t m_nCoverDeadlyHeavyLowRight;
    uint32_t m_nCoverDeadlyLowRight180;
    uint32_t m_nCoverDeadlyHeavyHighRight180;
    uint32_t m_nCoverDeadlyHeavyLowRight180;
    uint32_t m_nCoverDeadlyHeavyHighLeft180;
    uint32_t m_nCoverDeadlyHeavyLowLeft180;
    uint32_t m_nCoverDeadlyThrowHighLeft;
    uint32_t m_nCoverDeadlyThrowHighRight;
    uint32_t m_nCoverDeadlyThrowLowSM;
    uint32_t m_nCoverDeadlyThrowLowLeft;
    uint32_t m_nCoverDeadlyThrowLowRight;
    uint32_t m_nContextActionState;
    uint32_t m_nContextAction;
    uint32_t m_nInteractiveCutsequence;
    uint32_t m_nImpactBack;
    uint32_t m_nImpactFront;
    uint32_t m_nImpactLeft;
    uint32_t m_nImpactRight;
    uint32_t m_nLedgeAimFrontR;
    uint32_t m_nLedgeAimSideR;
    uint32_t m_nLedgeAimCrossR;
    uint32_t m_nLedgeAimCornerR;
    uint32_t m_nLedgeAimFrontL;
    uint32_t m_nLedgeAimSideL;
    uint32_t m_nLedgeAimCrossL;
    uint32_t m_nLedgeAimCornerL;
    uint32_t m_nLedgeAimWindowR;
    uint32_t m_nLedgeAimWindowL;
    uint32_t m_nHighAimPose;
    uint32_t m_nLowAimPose;
    uint32_t m_nHighAimPoseCover;
    uint32_t m_nLowAimPoseCover;
    bool m_bInitialized;
};

class ZHM5BaseInventory
{
  public:
    ZInventorySlot* AddItemToInventory(const TEntityRef<IHM5Item>& p_Item, bool p_AddInitialBullets, bool p_DisableHUD)
    {
        return Functions::ZHM5BaseInventory_AddItemToInventory->Call(this, p_Item, p_AddInitialBullets, p_DisableHUD);
    }

    void SetAmmoInPocket(TFixedArray<uint32_t, 8>& p_AmmoInPocket)
    {
        *reinterpret_cast<TFixedArray<uint32_t, 8>*>(m_nAmmoInPocket) = p_AmmoInPocket;
    }

  private:
    TArray<TEntityRef<IHM5Item>> m_Items;
    TArray<TEntityRef<IHM5Item>> m_RemoteControlledItems;
    uint32_t m_nAmmoInPocket[8];
};

class ZHM5FocusController
{
  public:
    void SetFocus(float p_Focus)
    {
        m_fPendingFocus = p_Focus;
    }

    bool m_bIsGaining;
    float m_fFocusGainTechniqueModifier;
    float m_fFocusMaxTechniqueModifier;
    float* m_pFocus;
    float m_fAdjustFocus;
    float m_fPendingMinFocus;
    float m_fPendingFocus;
    TEntityRef<ZHitman5> m_pHitman;
};

class ZHitman5 : public ZHM5BaseCharacter,
                 public IFutureCameraState,
                 public ICharacterCollision,
                 public IHM5ActionEntityListener,
                 public ZHM5MorphemeNodeIds
{
  public:
    void EquipOutfitResource(ZRuntimeResourceID p_NewOutfitLibrary, TEntityRef<IHM5Outfit> p_Outfit, bool p_PlaceClothBundle, bool p_Force)
    {
        Functions::ZHitman5_EquipOutfitResource->Call(this, p_NewOutfitLibrary, p_Outfit, p_PlaceClothBundle, p_Force);
    }

    PAD(0x3B0);                               // 0x680
    ZHM5InputControl* m_pInputControl;        // 0xA30
    PAD(0x10);                                // 0xA34
    ZHM5BaseInventory* m_pBaseInventory;      // 0xA44
    PAD(0x10);                                // 0xA48
    ZHM5FocusController* m_pFocusController;  // 0xA58
    PAD(0x238);                               // 0xA5C
    TEntityRef<ZHM5MainCamera> m_rMainCamera; // 0xC94
    PAD(0x84);                                // 0xC9C
};

static_assert(sizeof(ZHitman5) == 0xD20);
static_assert(alignof(ZHitman5) == 0x10);
