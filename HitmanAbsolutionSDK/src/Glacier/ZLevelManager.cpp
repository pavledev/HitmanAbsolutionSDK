#include <Glacier/ZLevelManager.h>

const SSceneParameters& ZLevelManager::GetSceneParameters() const
{
	return m_SceneTransitionData;
}

SSceneParameters& ZLevelManager::GetSceneParameters()
{
	return m_SceneTransitionData;
}

const TEntityRef<ZHitman5>& ZLevelManager::GetHitman() const
{
	return m_rHitman;
}

TEntityRef<ZOutfitManagerEntity> ZLevelManager::GetOutfitManager() const
{
	return m_pOutfitManager;
}
