#include <Glacier/Scene/ZEntitySceneContext.h>

TResourcePtr<IEntityBlueprintFactory>& ZEntitySceneContext::GetSceneBlueprintResource()
{
	return m_sceneBlueprintResource;
}

ZEntityRef ZEntitySceneContext::GetLoadedScene()
{
	return m_pLoadedScene;
}
