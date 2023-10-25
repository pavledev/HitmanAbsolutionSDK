#include <Glacier/Actor/ZHM5CCProfile.h>

ZHM5CCProfile* ZHM5CCProfile::GetDefaultCCProfile(EActorCCPreset eActorCCPreset)
{
    if (!s_apPresetCCProfile[0])
    {
        for (int i = 0; i < 9; ++i)
        {
            s_apPresetCCProfile[i] = *reinterpret_cast<ZHM5CCProfile**>(BaseAddress + 0xE235A8 + i * sizeof(ZHM5CCProfile*));
        }
    }

    if (eActorCCPreset >= ACCP_None)
    {
        return 0;
    }

    return s_apPresetCCProfile[eActorCCPreset];
}
