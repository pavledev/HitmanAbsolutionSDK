#pragma once

#include <map>

#include <Glacier/ZToken.h>
#include <Glacier/ZResource.h>
#include <Glacier/ZGameLoopManager.h>
#include <Glacier/ZInput.h>

#include <IModInterface.h>

class ZActor;
class ZDynamicResourceLibrary;
class ZEntityType;
class ZEntityManager;
class IEntityFactory;
class ZEntityRef;
class ZHM5WeaponControl;
class ZHM5ReloadController;

class ZTossGeometry;
class ZLevelDescriptor;

class Player : public IModInterface
{
  public:
    Player();
    ~Player() override;

    void Initialize() override;
    void OnEngineInitialized() override;
    void OnDrawMenu(IImGuiRenderer* p_Renderer) override;
    void OnDrawUI(IImGuiRenderer* p_Renderer, bool p_HasFocus) override;

  private:
    struct Outfit
    {
        std::string m_Title;
        STokenID m_TokenID;
        std::vector<std::pair<ZRuntimeResourceID, std::string>> m_OutfitVariations;
    };

    struct Firearm
    {
        std::string m_Title;
        ZRuntimeResourceID m_RuntimeResourceID;
    };

    struct Item
    {
        std::string m_Title;
        ZRuntimeResourceID m_RuntimeResourceID;
    };

    enum class ActorResourceType
    {
        Outfit,
        Model
    };

    enum class ItemDestination
    {
        World,
        Inventory
    };

    void OnFrameUpdate(const SGameUpdateEvent& p_UpdateEvent);

    void DrawCheatsTab();
    void DrawOutfitsTab();
    void DrawFirearmsTab();
    void DrawItemsTab();

    void LoadOufits();
    void LoadFirearms();
    void LoadItems();
    void LoadActorTypesAndResourceIDs();

    void EquipOutfit(ZActor* p_Actor);
    void EquipOutfit(const STokenID& p_TokenID, const ZRuntimeResourceID& p_OutfitVariation);

    void EquipModel(ZActor* p_Actor);
    void EquipModel(const std::string& p_ResourceID);

    void SpawnFirearm();
    void SpawnItem();

    void SetInfiniteAmmo();

    ZActor* FindNearestActor();

    DECLARE_THISCALL_MOD_DETOUR(Player, void, ZEntitySceneContext_ClearScene, ZEntitySceneContext* p_EntitySceneContext, bool p_FullyUnloadScene);

    DECLARE_THISCALL_MOD_DETOUR(
        Player, ZEntityType**, ZEntityManager_ConstructUninitializedEntity, ZEntityManager* p_EntityManager, const ZString& p_DebugName,
        IEntityFactory* p_EntityFactory, uint8_t* p_MemBlock
    );

    DECLARE_THISCALL_MOD_DETOUR(Player, void, ZHM5ReloadController_EndReloadWeapon, ZHM5ReloadController* p_HM5ReloadController);

    bool m_ShowWindow;

    bool m_IsGodModeEnabled;
    bool m_IsInvisibilityEnabled;
    bool m_IsInfiniteAmmoEnabled;

    std::vector<Outfit> m_Outfits;
    std::vector<Firearm> m_Firearms;
    std::vector<Item> m_Items;
    std::map<std::string, std::string> m_ActorTypeToResourceID;

    std::string m_ActorName;

    ZInputAction m_GetOutfitAction;
    ZInputAction m_GetModelAction;
    ZInputAction m_ChangeOutfitAction;
    ZInputAction m_TeleportAction;

    std::vector<ZDynamicResourceLibrary*> m_DynamicResourceLibraries;
    ZRuntimeResourceID m_SourceResourceRuntimeResourceID;

    ZRuntimeResourceID m_SelectedFirearm;
    bool m_SpawnFirearm;
    bool m_AddFirearmToWorld;
    int32_t m_FirearmSpawnCount;
    int32_t m_SpawnedFirearmCount;

    ZRuntimeResourceID m_SelectedItem;
    bool m_SpawnItem;
    bool m_AddItemToWorld;
    int32_t m_ItemSpawnCount;
    int32_t m_SpawnedItemCount;
};

DECLARE_HMASDK_MOD(Player)
