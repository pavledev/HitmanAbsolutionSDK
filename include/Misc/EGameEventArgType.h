#pragma once

enum EGameEventArgType : __int32
{
    GE_ARGS_KILL = 0x0,
    GE_ARGS_PRIORITYKILL = 0x1,
    GE_ARGS_SITUATION = 0x2,
    GE_ARGS_INVENTORY = 0x3,
    GE_ARGS_SHOOTINGBALLET = 0x4,
    GE_ARGS_EXPLODINGPROP = 0x5,
    GE_ARGS_EVENT = 0x6,
    GE_ARGS_CHECKPOINTEVENT = 0x7,
    GE_ARGS_COUNT = 0x8
};
