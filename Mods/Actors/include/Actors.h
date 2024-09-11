#pragma once

#include <Glacier/Resource/ZRuntimeResourceID.h>
#include <Glacier/STokenID.h>

#include <ModInterface.h>

class ZActor;
class ZDynamicResourceLibrary;
class ZEntityType;
class ZEntityManager;
class IEntityFactory;

void __fastcall ZEngineAppCommon_DefaultMainLoopSequenceHook(ZEngineAppCommon* pThis, int edx);

class Actors : public ModInterface
{
public:
    Actors();
    ~Actors() override;
    void Initialize() override;
    void OnDrawMenu() override;
    void OnDrawUI(const bool hasFocus) override;
    void OnDraw3D() override;

    void OnDefaultMainLoopSequence();

    void EquipModel(ZActor* actor, const std::string& resourceID);
    void SpawnWeapon(const ZRuntimeResourceID& runtimeResourceID);

    void GetWeapons();

private:
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
