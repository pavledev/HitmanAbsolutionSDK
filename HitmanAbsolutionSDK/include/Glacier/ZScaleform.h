#pragma once

#include "TFixedArray.h"

enum ECrossHairType
{
    CROSSHAIR_NONE = 0,
    CROSSHAIR_PISTOL = 1,
    CROSSHAIR_REVOLVER = 2,
    CROSSHAIR_SMG = 3,
    CROSSHAIR_RIFLE = 4,
    CROSSHAIR_SHOTGUN = 5,
    CROSSHAIR_SNIPER = 6,
    CROSSHAIR_HARDBALLER = 7,
    CROSSHAIR_UNAIMED = 8
};

enum EHUDFadeReason
{
    HUD_FADE_REASON_DIFFICULTY = 0,
    HUD_FADE_REASON_FADER = 1,
    HUD_FADE_REASON_TUTORIAL = 2,
    HUD_FADE_REASON_HINTMESSAGE = 3,
    HUD_FADE_REASON_CODE_GENERAL = 4,
    HUD_FADE_REASON_CUTSCENE = 5,
    HUD_FADE_REASON_NUM = 6
};

enum EHUDItem
{
    HUD_ITEM_TRESPASSING_ICON = 0,
    HUD_ITEM_BEING_WATCHED_ICON = 1,
    HUD_ITEM_AI_STATE_ICON = 2,
    HUD_ITEM_CLOSE_COMBAT = 3,
    HUD_ITEM_CLOSE_COMBAT_RESULT = 4,
    HUD_ITEM_WEAPON_SELECTION = 5,
    HUD_ITEM_WEAPON_DISPLAY = 6,
    HUD_ITEM_THREAT_RADAR = 7,
    HUD_ITEM_SHOW_THREATS = 8,
    HUD_ITEM_FOCUS_BAR = 9,
    HUD_ITEM_BOSS_HEALTHBAR = 10,
    HUD_ITEM_DARE_TIMER = 11,
    HUD_ITEM_PROFILE_DATA = 12,
    HUD_ITEM_DEBUGLOG = 13,
    HUD_ITEM_NUM = 14
};

class GFxValue
{
  public:
    enum ValueType
    {
        VT_Undefined = 0,
        VT_Null = 1,
        VT_Boolean = 2,
        VT_Number = 3,
        VT_String = 4,
        VT_StringW = 5,
        VT_Object = 6,
        VT_Array = 7,
        VT_DisplayObject = 8,
        VT_ConvertBoolean = 130,
        VT_ConvertNumber = 131,
        VT_ConvertString = 132,
        VT_ConvertStringW = 133
    };

    union ValueUnion
    {
        float NValue;
        bool BValue;
        const char* pString;
        const char** pStringManaged;
        const wchar_t* pStringW;
        void* pData;
    };

    class ObjectInterface;

    bool GetBool() const
    {
        return Value.BValue;
    }

    void SetBoolean(bool p_Value)
    {
        Value.BValue = p_Value;
    }

    bool GetMember(const char* p_Name, GFxValue* p_Value) const
    {
        return Functions::GFxValue_GetMember->Call(this, p_Name, p_Value);
    }

    bool SetMember(const char* p_Name, const GFxValue& p_Value)
    {
        return Functions::GFxValue_SetMember->Call(this, p_Name, p_Value);
    }

  private:
    ObjectInterface* pObjectInterface = nullptr;
    ValueType Type = VT_Undefined;
    ValueUnion Value;
};

class IScaleformExternalInterfaceHandler
{
  public:
    virtual ~IScaleformExternalInterfaceHandler() = default;
};

class IScaleformManager : public IComponentInterface
{};

class IScaleformPlayer : public IComponentInterface
{
  public:
    virtual void IScaleformPlayer_unk5() = 0;
    virtual void IScaleformPlayer_unk6() = 0;
    virtual void IScaleformPlayer_unk7() = 0;
    virtual void IScaleformPlayer_unk8() = 0;
    virtual void IScaleformPlayer_unk9() = 0;
    virtual void IScaleformPlayer_unk10() = 0;
    virtual void IScaleformPlayer_unk11() = 0;
    virtual void IScaleformPlayer_unk12() = 0;
    virtual void IScaleformPlayer_unk13() = 0;
    virtual void IScaleformPlayer_unk14() = 0;
    virtual void IScaleformPlayer_unk15() = 0;
    virtual bool GetMember(const char* param1, GFxValue* fxValue) = 0;
};

class ZGameWideUI : public IComponentInterface
{
  public:
    TEntityRef<ZGameWideUIScaleformHandler> m_rGameWideUI; // 0x4
};

class ZGameWideUIScaleformHandler : public ZEntityImpl, public IScaleformExternalInterfaceHandler
{
  public:
    void ShowUICursor(bool p_Show)
    {
        return Functions::ZGameWideUIScaleformHandler_ShowUICursor->Call(this, p_Show);
    }
};

class ZHUDManager : public IComponentInterface, public IScaleformExternalInterfaceHandler
{
  public:
    void ShowHUD(bool p_Show)
    {
        Functions::ZHUDManager_ShowHUD->Call(this, p_Show);
    }

    void ShowTrespassingIcon(bool p_Show)
    {
        Functions::ZHUDManager_ShowTrespassingIcon->Call(this, p_Show);
    }

    void ShowAIStateIcon(int32_t p_State)
    {
        Functions::ZHUDManager_ShowAIStateIcon->Call(this, p_State);
    }

    void ShowFocusBar(bool p_Show)
    {
        if (m_bDisplayHUD && p_Show != (m_aVisible[9] != 0))
        {
            FadeHUDElements(8, HUD_FADE_REASON_CODE_GENERAL, p_Show, 0.f);

            m_aVisible[9] = p_Show;
        }
    }

    void ShowThreatRadar(bool p_Show)
    {
        Functions::ZHUDManager_ShowThreatRadar->Call(this, p_Show);
    }

    void ShowCrossHair()
    {
        m_eCrossHair = CROSSHAIR_PISTOL;
    }

    void HideCrossHair()
    {
        m_eCrossHair = CROSSHAIR_NONE;
    }

    void FadeHUDElements(uint32_t p_Elements, EHUDFadeReason p_Reason, bool p_FadeIn, float p_Time)
    {
        Functions::ZHUDManager_FadeHUDElements->Call(this, p_Elements, p_Reason, p_FadeIn, p_Time);
    }

    void ScaleformShowWeaponDisplay(bool p_Show)
    {
        Functions::ZHUDManager_ScaleformShowWeaponDisplay->Call(this, p_Show);
    }

    void HideRatingTracker()
    {
        Functions::ZHUDManager_HideRatingTracker->Call(this);
    }

    PAD(0x4);                                      // 0x8
    IScaleformPlayer* m_mcHUD;                     // 0xC
    PAD(0x30);                                     // 0x10
    TFixedArray<int32_t, HUD_ITEM_NUM> m_aVisible; // 0x40
    PAD(0x1);                                      // 0x78
    bool m_bDisplayHUD;                            // 0x79
    PAD(0x2);                                      // 0x7A
    bool m_bPauseMenuActive;                       // 0x7C
    PAD(0x5A3);                                    // 0x7D
    ECrossHairType m_eCrossHair;                   // 0x620
};

class ZScaleformManager : public IScaleformManager
{
  public:
    PAD(0xEB);            // 0x4
    bool m_bIsInMainMenu; // 0xEF
};