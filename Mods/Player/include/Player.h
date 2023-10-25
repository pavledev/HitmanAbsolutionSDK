#pragma once

#include <Glacier/STokenID.h>
#include <Glacier/Resource/ZRuntimeResourceID.h>
#include <Glacier/ZGameLoopManager.h>
#include <Glacier/Input/ZInputAction.h>

#include <ModInterface.h>

class ZActor;
class ZDynamicResourceLibrary;
class ZEntityType;
class ZEntityManager;
class IEntityFactory;
class ZEntityRef;

void __fastcall ZEngineAppCommon_DefaultMainLoopSequenceHook(ZEngineAppCommon* pThis, int edx);
ZEntityType** __fastcall ZEntityManager_ConstructUninitializedEntityHook(ZEntityManager* pThis, int edx, const ZString& sDebugName, IEntityFactory* pEntityFactory, unsigned char* pMemBlock);

class Player : public ModInterface
{
public:
    Player();
    ~Player() override;
    void Initialize() override;
    void OnEngineInitialized() override;
    void OnDrawMenu() override;
    void OnDrawUI(const bool hasFocus) override;

    void OnDefaultMainLoopSequence();
    void OnConstructUninitializedEntity(IEntityFactory* pEntityFactory, ZEntityType** entityType);

private:
    struct OutfitKitEntity
    {
        ~OutfitKitEntity();

        std::string title;
        std::string description;
        STokenID tokenID;
        ZRuntimeResourceID hiResNotebookImage;
        ID3D11Resource* texture = nullptr;
        ID3D11ShaderResourceView* textureView = nullptr;
        float width = 0;
        float height = 0;
        bool isOutfitForCurrentScene = false;
        std::vector<ZRuntimeResourceID> outfitVariations;
    };

    struct FireArmKitEntity
    {
        ~FireArmKitEntity();

        std::string title;
        std::string description;
        ZRuntimeResourceID hiResNotebookImage;
        ID3D11Resource* texture = nullptr;
        ID3D11ShaderResourceView* textureView = nullptr;
        float width = 0;
        float height = 0;
        ZRuntimeResourceID runtimeResourceID;
    };

    struct PropKitEntity
    {
        ~PropKitEntity();

        std::string title;
        std::string description;
        ZRuntimeResourceID hiResNotebookImage;
        ID3D11Resource* texture = nullptr;
        ID3D11ShaderResourceView* textureView = nullptr;
        float width = 0;
        float height = 0;
        ZRuntimeResourceID runtimeResourceID;
    };

    void OnFrameUpdate(const SGameUpdateEvent& updateEvent);

    void RenderCheatsTabItem();
    void RenderOutfitsTabItem();
    void RenderWeaponsTabItem();
    void RenderItemsTabItem();
    void RenderPropsTabItem();
    void RenderActorsTabItem();

    void GetOufits();
    void GetWeapons();
    void GetItems();
    void EquipOutfit(ZActor* actor);
    void EquipOutfit(const unsigned int outfitIndex, const unsigned int outfitVariationIndex);
    void EquipModel(ZActor* actor);
    void EquipModel(const std::string& resourceID);
    void SpawnWeapon(const ZRuntimeResourceID& runtimeResourceID, const unsigned int weaponCount, const bool addToWorld);
    void SpawnWeapon(const ZRuntimeResourceID& runtimeResourceID, ZActor* actor);
    void SpawnItem(const ZRuntimeResourceID& runtimeResourceID, const unsigned int itemCount, const bool addToWorld);
    void SpawnProp();
    void SpawnActor();

    void LoadDDSTexture(const ZRuntimeResourceID& hiResNotebookImage, ID3D11Resource** texture, ID3D11ShaderResourceView** textureView, float& width, float& height);

    ZActor* FindNearestActor();

    void SetPropertiesForCivilianActor(ZEntityRef& entityRef);
    void SetPropertiesForGuardActor(ZEntityRef& entityRef);

    bool isOpen;

    bool isGodModeEnabled;
    bool isInvisibilityEnabled;
    bool isInfAmmoEnabled;

    inline static int* godMode;
    inline static int* invisible;
    inline static int* infAmmo;

    std::vector<OutfitKitEntity> outfitKitEntities;
    std::vector<FireArmKitEntity> fireArmKitEntities;
    std::vector<PropKitEntity> propKitEntities;
    int selectedOutfitIndex;
    int selectedWeaponIndex;
    int selectedItemIndex;

    ZInputAction getOutfitAction;
    ZInputAction getModelAction;
    ZInputAction changeOutfitAction;
    ZInputAction teleportAction;

    std::vector<ZDynamicResourceLibrary*> dynamicResourceLibraries;

    bool spawnWeapon;
    bool spawnItem;
    bool spawnProp;
    bool spawnActor;
    unsigned int weaponCount;
    unsigned int itemCount;
    unsigned int propCount;
    unsigned int actorCount;
    bool addWeaponToWorld;
    bool addItemToWorld;
    char propResourceID[512]{ "" };
    char actorResourceID[512]{ "" };
    char actorName[50]{ "" };
    bool spawnCivilianActor;
    int selectedActorWeaponIndex;
    ZRuntimeResourceID sourceResourceRuntimeResourceID;
};

DECLARE_MOD(Player)
