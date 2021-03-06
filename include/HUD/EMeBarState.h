#pragma once

enum EMeBarState : __int32
{
    ME_BAR_NONE = 0x0,
    ME_BAR_TRESPASSING = 0x1,
    ME_BAR_DEEPTRESPASSING = 0x2,
    ME_BAR_VISIBLY_ARMED = 0x3,
    ME_BAR_CHANGING_DISGUISE = 0x4,
    ME_BAR_DISGUISE_BLOWN = 0x5,
    ME_BAR_DISGUISE_SUSPICIOUS = 0x6,
    ME_BAR_NEAR_BODY = 0x7
};
