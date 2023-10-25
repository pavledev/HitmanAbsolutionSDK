#include <Glacier/ContentKit/ZGlobalOutfitKit.h>

ZRuntimeResourceID ZGlobalOutfitKit::GetVariationResource(int nVariation)
{
    ZRuntimeResourceID result;

    switch (nVariation)
    {
        case 1:
            result = m_OutfitResource1;
            break;
        case 2:
            result = m_OutfitResource2;
            break;
        case 3:
            result = m_OutfitResource3;
            break;
        case 4:
            result = m_OutfitResource4;
            break;
        case 5:
            result = m_OutfitResource5;
            break;
        case 6:
            result = m_OutfitResource6;
            break;
        case 7:
            result = m_OutfitResource7;
            break;
        case 8:
            result = m_OutfitResource8;
            break;
        case 9:
            result = m_OutfitResource9;
            break;
        case 10:
            result = m_OutfitResource10;
            break;
        case 11:
            result = m_OutfitResource11;
            break;
        case 12:
            result = m_OutfitResource12;
            break;
    }

    return result;
}
