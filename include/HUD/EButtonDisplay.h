#pragma once

enum EButtonDisplay : __int32
{
    BUTTON_DISPLAY_AT_OPENDOOR = 0x0,
    BUTTON_DISPLAY_AT_PICKUP = 0x1,
    BUTTON_DISPLAY_AT_STARTDIALOG = 0x2,
    BUTTON_DISPLAY_AT_RESET = 0x3,
    BUTTON_DISPLAY_AT_OUTFIT = 0x4,
    BUTTON_DISPLAY_AT_CLOTHBUNDLE = 0x5,
    BUTTON_DISPLAY_AT_DRAGBODY = 0x6,
    BUTTON_DISPLAY_AT_HUMANSHIELD = 0x7,
    BUTTON_DISPLAY_AT_RELEASEHUMANSHIELD = 0x8,
    BUTTON_DISPLAY_AT_KILLHUMANSHIELD = 0x9,
    BUTTON_DISPLAY_AT_SWITCHBOX = 0xA,
    BUTTON_DISPLAY_AT_SWAPITEM = 0xB,
    BUTTON_DISPLAY_AT_DUMPBODY = 0xC,
    BUTTON_DISPLAY_AT_OPENLID = 0xD,
    BUTTON_DISPLAY_AT_CHECKPOINTDOOR = 0xE,
    BUTTON_DISPLAY_AT_SIMPLE = 0xF,
    BUTTON_DISPLAY_AT_ENTERCLOSET = 0x10,
    BUTTON_DISPLAY_AT_EXITCLOSET = 0x11,
    BUTTON_DISPLAY_AT_ENTERSAFEZONE = 0x12,
    BUTTON_DISPLAY_AT_EXITSAFEZONE = 0x13,
    BUTTON_DISPLAY_AT_DISABLEFUSEBOX = 0x14,
    BUTTON_DISPLAY_AT_CONTAINERFLUSHBODY = 0x15,
    BUTTON_DISPLAY_AT_CONTEXTACTION = 0x16,
    BUTTON_DISPLAY_AT_VENTILATORSHAFT = 0x17,
    BUTTON_DISPLAY_AT_HEALTHSTATION = 0x18,
    BUTTON_DISPLAY_AT_KEYCARDREADER = 0x19,
    BUTTON_DISPLAY_AT_USE = 0x1A,
    BUTTON_DISPLAY_SHOOTING_BALLET = 0x1B,
    BUTTON_DISPLAY_SHOOTING_BALLET_START = 0x1C,
    BUTTON_DISPLAY_SHOOTING_BALLET_ZOOM = 0x1D,
    BUTTON_DISPLAY_SHOOTING_BALLET_TAG = 0x1E,
    BUTTON_DISPLAY_SHOOTING_BALLET_CANCEL = 0x1F,
    BUTTON_DISPLAY_SHOOTING_BALLET_SLOWDOWN = 0x20,
    BUTTON_DISPLAY_SHOOTING_BALLET_CINEMATIC = 0x21,
    BUTTON_DISPLAY_SHOOTING_BALLET_HOLD_CAMERA = 0x22,
    BUTTON_DISPLAY_SHOOTING_BALLET_NEXT_CAMERA = 0x23,
    BUTTON_DISPLAY_RELOAD = 0x24,
    BUTTON_DISPLAY_HOLSTER = 0x25,
    BUTTON_DISPLAY_CHANGE_WEAPON_MODE = 0x26,
    BUTTON_DISPLAY_REMOTE_DETONATE = 0x27,
    BUTTON_DISPLAY_INVENTORY_ACCEPT = 0x28,
    BUTTON_DISPLAY_INVENTORY_CANCEL = 0x29,
    BUTTON_DISPLAY_HINTMESSAGE_CONTINUE = 0x2A,
    BUTTON_DISPLAY_CLOSECOMBAT_TAKEDOWN = 0x2B,
    BUTTON_DISPLAY_CLOSECOMBAT_COUNTERATTACK = 0x2C,
    BUTTON_DISPLAY_CLOSECOMBAT_SILENT = 0x2D,
    BUTTON_DISPLAY_CLOSECOMBAT_SNAPNECK = 0x2E,
    BUTTON_DISPLAY_CLOSECOMBAT_CHOKE = 0x2F,
    BUTTON_DISPLAY_CLOSECOMBAT_CHOKE_TARGET = 0x30,
    BUTTON_DISPLAY_CLOSECOMBAT_CHAIN_HIT = 0x31,
    BUTTON_DISPLAY_PISTOL_FINISHER = 0x32,
    BUTTON_DISPLAY_PEEK_VENT = 0x33,
    BUTTON_DISPLAY_PEEK_CLOSET = 0x34,
    BUTTON_DISPLAY_CLIMB_LADDER = 0x35,
    BUTTON_DISPLAY_SLIDE_LADDER = 0x36,
    BUTTON_DISPLAY_LEDGE = 0x37,
    BUTTON_DISPLAY_LEDGE_STEPON = 0x38,
    BUTTON_DISPLAY_LEDGEJUMP = 0x39,
    BUTTON_DISPLAY_LEDGE_PULL_UP = 0x3A,
    BUTTON_DISPLAY_LEDGE_CLIMB_DOWN = 0x3B,
    BUTTON_DISPLAY_LEDGE_DROP_DOWN = 0x3C,
    BUTTON_DISPLAY_WINDOW = 0x3D,
    BUTTON_DISPLAY_WINDOWSNEAK = 0x3E,
    BUTTON_DISPLAY_PULLVICTIM = 0x3F,
    BUTTON_DISPLAY_PUSHVICTIM = 0x40,
    BUTTON_DISPLAY_SURRENDER = 0x41,
    BUTTON_DISPLAY_COVERTOCOVER = 0x42,
    BUTTON_DISPLAY_COVERCORNER = 0x43,
    BUTTON_DISPLAY_COVEREXITFORWARD = 0x44,
    BUTTON_DISPLAY_COVERSCALEEXIT = 0x45,
    BUTTON_DISPLAY_TAKEDISGUISE = 0x46,
    BUTTON_DISPLAY_VENTILATOR_EXIT = 0x47,
    BUTTON_DISPLAY_DROPBODY = 0x48,
    BUTTON_DISPLAY_ABORT_CHECKPOINTDOOR = 0x49,
    BUTTON_DISPLAY_QUICKSWAP = 0x4A,
    BUTTON_DISPLAY_OPERATE_LEVER = 0x4B,
    BUTTON_DISPLAY_CLOSECOMBAT_CHAIN_HIGH = 0x4C,
    BUTTON_DISPLAY_CLOSECOMBAT_CHAIN_LOW = 0x4D,
    BUTTON_DISPLAY_CLOSECOMBAT_CHAIN_COUNTER = 0x4E,
    BUTTON_DISPLAY_FIBERWIRE_KILL = 0x4F,
    BUTTON_DISPLAY_DROP_ITEM = 0x50,
    BUTTON_DISPLAY_ABORT_MINIGAME = 0x51,
    BUTTON_DISPLAY_CONTRACT_MARK = 0x52,
    BUTTON_DISPLAY_CONTRACT_UNMARK = 0x53,
    BUTTON_DISPLAY_MAX = 0x54
};
