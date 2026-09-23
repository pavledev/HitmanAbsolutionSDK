#pragma once

#include <Glacier/ZResource.h>
#include <Glacier/ZToken.h>

#include <IModInterface.h>

class ZDynamicResourceLibrary;

class Actors : public IModInterface
{
  public:
    Actors();
    ~Actors() override;
    void OnEngineInitialized() override;
    void OnDrawMenu(IImGuiRenderer* p_Renderer) override;
    void OnDrawUI(IImGuiRenderer* p_Renderer, bool p_HasFocus) override;
    void OnDraw3D(IDirectXRenderer* p_Renderer) override;

  private:
    void OnFrameUpdate(const SGameUpdateEvent& updateEvent);

    void EquipModel(ZActor* actor, const std::string& resourceID);
    void SpawnWeapon(const ZRuntimeResourceID& runtimeResourceID);

    void GetWeapons();

    struct FireArmKitEntity
    {
        std::string title;
        STokenID tokenID;
        ZRuntimeResourceID runtimeResourceID;
    };

    bool isOpen;
    int selectedActorIndex;
    std::vector<ZDynamicResourceLibrary*> dynamicResourceLibraries;
    bool renderActorNames;
    bool isGodModeEnabled;
    std::vector<FireArmKitEntity> fireArmKitEntities;
    bool spawnWeapon;
    int selectedActorWeaponIndex;
};

DECLARE_HMASDK_MOD(Actors)
