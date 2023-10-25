#pragma once

#include "../IComponentInterface.h"
#include "../Templates/TSList.h"
#include "../ZGameTime.h"
#include "ZHM5Action.h"

class ZAABBTreeNode;

class ZHM5ActionManager : public IComponentInterface
{
public:
	struct SActionTreeEntry
	{
		ZHM5Action* m_pNodeAction;
		void* m_pObjectNode;
	};

	TSList<SActionTreeEntry> m_Actions;
	ZAABBTreeNode* m_pAABBActionTree;
	TSList<ZHM5Action*> m_ObjectlessActions;
	ZGameTime m_LastUpdate;
};
