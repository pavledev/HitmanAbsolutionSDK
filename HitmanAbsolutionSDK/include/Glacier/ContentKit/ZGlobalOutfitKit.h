#pragma once

#include "../Entity/ZEntityImpl.h"
#include "../STokenID.h"
#include "../Templates/TResourcePtr.h"
#include "../Resource/ZRuntimeResourceID.h"

class ZEntityRef;

class HitmanAbsolutionSDK_API ZGlobalOutfitKit : public ZEntityImpl
{
public:
	ZRuntimeResourceID GetVariationResource(int nVariation);

private:
	STokenID m_TokenId;
	TResourcePtr<ZEntityRef> m_TokenResource;
	ZRuntimeResourceID m_OutfitResource1;
	ZRuntimeResourceID m_OutfitResource2;
	ZRuntimeResourceID m_OutfitResource3;
	ZRuntimeResourceID m_OutfitResource4;
	ZRuntimeResourceID m_OutfitResource5;
	ZRuntimeResourceID m_OutfitResource6;
	ZRuntimeResourceID m_OutfitResource7;
	ZRuntimeResourceID m_OutfitResource8;
	ZRuntimeResourceID m_OutfitResource9;
	ZRuntimeResourceID m_OutfitResource10;
	ZRuntimeResourceID m_OutfitResource11;
	ZRuntimeResourceID m_OutfitResource12;
};
