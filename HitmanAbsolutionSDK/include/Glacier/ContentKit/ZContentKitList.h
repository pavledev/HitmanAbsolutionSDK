#pragma once

#include "ZShadowContentKitList.h"
#include "../Templates/TMap.h"

struct STokenID;

template <typename A, typename B, typename C, typename D>
class ZContentKitList
{
public:
	const ZShadowContentKitList& GetShadowContentKitList() const
	{
		return m_List;
	}

private:
	ZShadowContentKitList m_List;
	TMap<STokenID, D> m_PersistentState;
};
