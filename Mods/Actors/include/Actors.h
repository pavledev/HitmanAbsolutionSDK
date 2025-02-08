#pragma once

#include <Glacier/Resource/ZRuntimeResourceID.h>
#include <Glacier/STokenID.h>

#include <ModInterface.h>

class ZDynamicResourceLibrary;

class Actors : public ModInterface
{
public:
    Actors();
    ~Actors() override;
    void Initialize() override;
    void OnEngineInitialized() override;
    void OnDrawMenu() override;
    void OnDrawUI(const bool hasFocus) override;
    void OnDraw3D() override;

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

    inline static int* godMode;
};

DECLARE_MOD(Actors)
