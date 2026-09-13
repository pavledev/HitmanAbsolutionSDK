#pragma once

#include "IComponentInterface.h"
#include "ZScene.h"

#include <Common.h>

class ZEntitySceneContext;

class IModule : public IComponentInterface
{};

class ZConfiguration
{};

class ZSimpleModuleBase : public IModule
{};

class ZHitman5Module : public ZSimpleModuleBase, public ZConfiguration
{
  public:
    bool IsEngineInitialized() const
    {
        if (!m_pSceneContext)
        {
            return false;
        }

        return m_pSceneContext->m_SceneInitParameters.m_SceneResource.Length() != 0;
    }

    PAD(0x44);
    ZEntitySceneContext* m_pSceneContext;
};
