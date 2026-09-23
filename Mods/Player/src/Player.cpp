#include "Player.h"

#include <fstream>

#include <IconsMaterialDesign.h>

#include <rapidjson/istreamwrapper.h>
#include <rapidjson/document.h>

#include <Glacier/ZLevelManager.h>
#include <Glacier/ZContentKit.h>
#include <Glacier/ZResource.h>
#include <Glacier/ZActor.h>
#include <Glacier/ZInput.h>
#include <Glacier/ZItem.h>
#include <Glacier/ZCheckPoint.h>
#include <Glacier/ZEntity.h>
#include <Glacier/TFixedArray.h>
#include <Glacier/ZDynamicResourceLibrary.h>

#include <Utils/ImGuiUtils.h>
#include <Hooks.h>
#include <Resources.h>

Player::Player()
{
    m_ShowWindow = false;

    m_IsGodModeEnabled = false;
    m_IsInvisibilityEnabled = false;
    m_IsInfiniteAmmoEnabled = false;

    m_GetOutfitAction = ZInputAction("GetOufit");
    m_GetModelAction = ZInputAction("GetModel");
    m_ChangeOutfitAction = ZInputAction("ChangeOutfit");
    m_TeleportAction = ZInputAction("TeleportHitman");

    m_SpawnFirearm = false;
    m_AddFirearmToWorld = false;
    m_FirearmSpawnCount = 1;
    m_SpawnedFirearmCount = 1;

    m_SpawnItem = false;
    m_AddItemToWorld = false;
    m_ItemSpawnCount = 1;
    m_SpawnedItemCount = 1;
}

Player::~Player()
{
    const ZMemberDelegate<Player, void(const SGameUpdateEvent&)> delegate(this, &Player::OnFrameUpdate);
    Globals::GameLoopManager->UnregisterForFrameUpdate(delegate);

    for (auto dynamicResourceLibrary : m_DynamicResourceLibraries)
    {
        dynamicResourceLibrary->~ZDynamicResourceLibrary();
        (*Globals::MemoryManager)->m_pNormalAllocator->Free(dynamicResourceLibrary);
    }
}

void Player::Initialize()
{
    Hooks::ZEntitySceneContext_ClearScene->AddDetour(this, &Player::ZEntitySceneContext_ClearScene);
    Hooks::ZEntityManager_ConstructUninitializedEntity->AddDetour(this, &Player::ZEntityManager_ConstructUninitializedEntity);
    Hooks::ZHM5ReloadController_EndReloadWeapon->AddDetour(this, &Player::ZHM5ReloadController_EndReloadWeapon);
}

void Player::OnEngineInitialized()
{
    const ZMemberDelegate<Player, void(const SGameUpdateEvent&)> delegate(this, &Player::OnFrameUpdate);
    Globals::GameLoopManager->RegisterForFrameUpdate(delegate, 1);

    const char* bindings = "PlayerInput={"
                           "GetOufit=tap(kb,i);"
                           "GetModel=tap(kb,o);"
                           "ChangeOutfit=tap(kb,p);"
                           "TeleportHitman=tap(kb,j);};";

    Globals::InputActionManager->AddBindings(bindings);
}

void Player::OnDrawMenu(IImGuiRenderer* p_Renderer)
{
    if (ImGui::Button(ICON_MD_MAN " Player"))
    {
        m_ShowWindow = !m_ShowWindow;
    }
}

void Player::OnDrawUI(IImGuiRenderer* p_Renderer, bool p_HasFocus)
{
    if (!p_HasFocus || !m_ShowWindow)
    {
        return;
    }

    ImGui::PushFont(p_Renderer->GetBlackFont());
    ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);

    const bool isWindowExpanded = ImGui::Begin(ICON_MD_TOKEN " Player", &m_ShowWindow);

    ImGui::PushFont(p_Renderer->GetRegularFont());

    if (isWindowExpanded)
    {
        if (m_Outfits.size() == 0)
        {
            LoadOufits();
        }

        if (m_Firearms.size() == 0)
        {
            LoadFirearms();
        }

        if (m_Items.size() == 0)
        {
            LoadItems();
        }

        if (m_ActorTypeToResourceID.size() == 0)
        {
            LoadActorTypesAndResourceIDs();
        }

        if (ImGui::BeginTabBar("##TabBar"))
        {
            if (ImGui::BeginTabItem("Cheats"))
            {
                DrawCheatsTab();

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Outfits"))
            {
                DrawOutfitsTab();

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Firearms"))
            {
                DrawFirearmsTab();

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Items"))
            {
                DrawItemsTab();

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
    }

    ImGui::PopFont();
    ImGui::End();
    ImGui::PopFont();
}

void Player::OnFrameUpdate(const SGameUpdateEvent& p_UpdateEvent)
{
    ZHitman5* hitman = Globals::LevelManager->m_rHitman.m_pInterfaceRef;

    if (!hitman)
    {
        return;
    }

    /*if (m_GetOutfitAction.Digital())
    {
        ZActor* actor = FindNearestActor();

        if (actor)
        {
            EquipOutfit(actor);
        }
    }

    if (m_GetModelAction.Digital())
    {
        ZActor* actor = FindNearestActor();

        if (actor)
        {
            EquipModel(actor);
        }
    }

    if (m_ChangeOutfitAction.Digital())
    {
        if (m_Outfits.size() == 0)
        {
            LoadOufits();
        }

        static unsigned int outfitIndex = 0;
        static unsigned int outfitVariationIndex = 0;

        if (outfitVariationIndex == m_Outfits[outfitIndex].m_OutfitVariations.size())
        {
            ++outfitIndex;
            outfitVariationIndex = 0;
        }

        if (outfitIndex == m_Outfits.size())
        {
            outfitIndex = 0;
        }

        EquipOutfit(outfitIndex, outfitVariationIndex);

        ++outfitVariationIndex;
    }

    if (m_TeleportAction.Digital())
    {
        ZCheckPointManagerEntity* checkPointManagerEntity = Globals::CheckPointManager->m_pCheckPointManagerEntity.m_pInterfaceRef;

        checkPointManagerEntity->ActivateJumpPoint(checkPointManagerEntity->m_iCurrentJumpPoint + 1, true);
    }*/

    if (m_SpawnFirearm)
    {
        SpawnFirearm();

        if (!m_AddFirearmToWorld)
        {
            m_SpawnFirearm = false;
        }
    }
    else if (m_SpawnItem)
    {
        SpawnItem();

        if (!m_AddItemToWorld)
        {
            m_SpawnItem = false;
        }
    }
}

void Player::DrawCheatsTab()
{
    if (ImGui::Checkbox("God mode", &m_IsGodModeEnabled))
    {
        *Globals::GodMode = static_cast<int32_t>(m_IsGodModeEnabled);
    }

    if (ImGui::Checkbox("Invisible", &m_IsInvisibilityEnabled))
    {
        *Globals::Invisible = static_cast<int32_t>(m_IsInvisibilityEnabled);
    }

    if (ImGui::Checkbox("Infinite ammo", &m_IsInfiniteAmmoEnabled))
    {
        if (m_IsInfiniteAmmoEnabled)
        {
            SetInfiniteAmmo();
        }
    }

    if (ImGui::Button("Refill focus"))
    {
        if (Globals::LevelManager)
        {
            ZHitman5* hitman = Globals::LevelManager->m_rHitman.m_pInterfaceRef;

            if (hitman)
            {
                hitman->m_pFocusController->SetFocus(1.0f);
            }
        }
    }
}

void Player::DrawOutfitsTab()
{
    static char outfitName[256]{ "" };
    static char variation[256]{ "" };
    static bool showOnlyOutfitsOfCurrentScene = false;

    static const Outfit* outfit = nullptr;

    ImGui::Checkbox("Show only outfits of current scene", &showOnlyOutfitsOfCurrentScene);

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Oufit");

    ImGui::SameLine();

    util::InputWithAutocomplete(
        "##Outfit", outfitName, sizeof(outfitName), m_Outfits, [](const auto& p_Outfit) -> const std::string& { return p_Outfit.m_Title; },
        [](const auto& p_Outfit) -> const std::string& { return p_Outfit.m_Title; },
        [&](const std::string&, const std::string& p_Name, const Outfit& p_Outfit)
        {
            outfit = &p_Outfit;
            variation[0] = '\0';
        },
        nullptr,
        [&](const auto& p_Outfit)
        {
            if (!showOnlyOutfitsOfCurrentScene)
            {
                return true;
            }

            for (const auto& entry : Globals::ContentKitManager->m_Outfits.m_List.m_List)
            {
                const auto outfitKitEntity = static_cast<ZOutfitKitEntity*>(entry.GetFirstKit().m_pInterfaceRef);

                if (p_Outfit.m_TokenID == outfitKitEntity->GetTokenID())
                {
                    return true;
                }
            }

            return false;
        }
    );

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Variation");

    ImGui::SameLine();

    util::InputWithAutocomplete(
        "##Variation", variation, sizeof(variation), outfit ? outfit->m_OutfitVariations : std::vector<std::pair<ZRuntimeResourceID, std::string>>{},
        [](const auto& p_Pair) -> std::string { return p_Pair.second; }, [](const auto& p_Pair) -> std::string { return p_Pair.second; },
        [&](const std::string&, const std::string&, const auto& p_Pair) { EquipOutfit(outfit->m_TokenID, p_Pair.first); }
    );

    ImGui::Separator();

    ImGui::Text("Get outfit/model from actor");
    ImGui::Spacing();

    static ActorResourceType resourceType = ActorResourceType::Outfit;

    if (ImGui::RadioButton("Outfit", resourceType == ActorResourceType::Outfit))
    {
        resourceType = ActorResourceType::Outfit;
    }

    ImGui::SameLine();

    if (ImGui::RadioButton("Model", resourceType == ActorResourceType::Model))
    {
        resourceType = ActorResourceType::Model;
    }

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Name");
    ImGui::SameLine();

    util::InputWithAutocomplete(
        "##Name", m_ActorName, Globals::ActorManager->m_aliveActors,
        [](const auto& p_Actor) -> std::string { return p_Actor.m_pInterfaceRef->m_sActorName.ToCString(); },
        [](const auto& p_Actor) -> std::string { return p_Actor.m_pInterfaceRef->m_sActorName.ToCString(); },
        [&](const std::string&, const std::string& p_Name, const auto& p_Actor)
        {
            if (resourceType == ActorResourceType::Outfit)
            {
                EquipOutfit(p_Actor.m_pInterfaceRef);
            }
            else
            {
                EquipModel(p_Actor.m_pInterfaceRef);
            }
        }
    );

    ImGui::Separator();

    ImGui::Text("Equip model");
    ImGui::Spacing();

    static char actorType[512]{ "" };

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Actor type");
    ImGui::SameLine();

    util::InputWithAutocomplete(
        "##ActorType", actorType, sizeof(actorType), m_ActorTypeToResourceID, [](const auto& p_Pair) -> const std::string& { return p_Pair.first; },
        [](const auto& p_Pair) -> const std::string& { return p_Pair.first; },
        [&](const std::string&, const std::string& p_Name, const auto& p_Pair) { EquipModel(p_Pair.second); }
    );

    ImGui::Separator();

    if (ImGui::Button("Get Nearest Actor's Outfit"))
    {
        ZActor* actor = FindNearestActor();

        if (actor)
        {
            EquipOutfit(actor);
        }
    }

    if (ImGui::Button("Get Nearest Actor's Model"))
    {
        ZActor* actor = FindNearestActor();

        if (actor)
        {
            EquipModel(actor);
        }
    }
}

void Player::DrawFirearmsTab()
{
    static char firearm[256]{ "" };
    static ItemDestination itemDestination = ItemDestination::World;

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Firearm");

    util::InputWithAutocomplete(
        "##Weapon", firearm, sizeof(firearm), m_Firearms, [](const auto& p_Firearm) -> const std::string& { return p_Firearm.m_Title; },
        [](const auto& p_Firearm) -> const std::string& { return p_Firearm.m_Title; },
        [&](const std::string&, const std::string& p_Name, const Firearm& p_Firearm)
        {
            m_SelectedFirearm = p_Firearm.m_RuntimeResourceID;
            m_AddFirearmToWorld = itemDestination == ItemDestination::World;
            m_SpawnFirearm = true;
        }
    );

    ImGui::Spacing();

    if (ImGui::RadioButton("Add To World", itemDestination == ItemDestination::World))
    {
        itemDestination = ItemDestination::World;
    }

    ImGui::SameLine();

    if (ImGui::RadioButton("Add To Inventory", itemDestination == ItemDestination::Inventory))
    {
        itemDestination = ItemDestination::Inventory;
    }

    ImGui::Spacing();

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Number of firearms to spawn");

    ImGui::SameLine();

    ImGui::SetNextItemWidth(ImGui::GetFrameHeight() * 5.f);

    ImGui::InputInt("##NumberOfFirearmsToSpawn", &m_FirearmSpawnCount);
}

void Player::DrawItemsTab()
{
    static char item[256]{ "" };
    static ItemDestination itemDestination = ItemDestination::World;

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Item");

    util::InputWithAutocomplete(
        "##Weapon", item, sizeof(item), m_Items, [](const auto& p_Item) -> const std::string& { return p_Item.m_Title; },
        [](const auto& p_Item) -> const std::string& { return p_Item.m_Title; },
        [&](const std::string&, const std::string& p_Name, const Item& p_Item)
        {
            m_SelectedItem = p_Item.m_RuntimeResourceID;
            m_AddItemToWorld = itemDestination == ItemDestination::World;
            m_SpawnItem = true;
        }
    );

    ImGui::Spacing();

    if (ImGui::RadioButton("Add To World", itemDestination == ItemDestination::World))
    {
        itemDestination = ItemDestination::World;
    }

    ImGui::SameLine();

    if (ImGui::RadioButton("Add To Inventory", itemDestination == ItemDestination::Inventory))
    {
        itemDestination = ItemDestination::Inventory;
    }

    ImGui::Spacing();

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Number of items to spawn");

    ImGui::SameLine();

    ImGui::InputInt("##NumberOfItemsToSpawn", &m_ItemSpawnCount);
}

void Player::LoadOufits()
{
    const std::string_view outfitsJson = SDK().GetTextResource(IDR_OUTFITS);

    if (outfitsJson.empty())
    {
        Logger::Error("Failed to load outfits resource.");
        return;
    }

    rapidjson::Document document;
    document.Parse(outfitsJson.data(), outfitsJson.size());

    if (document.HasParseError())
    {
        Logger::Error("Failed to parse outfits resource.");
        return;
    }

    const auto& outfits = document["outfits"].GetArray();

    m_Outfits.reserve(outfits.Size());

    for (const auto& object : outfits)
    {
        Outfit& outfit = m_Outfits.emplace_back();

        outfit.m_Title = object["title"].GetString();
        outfit.m_TokenID = STokenID(object["hash"].GetUint());

        // Hope Cougar outfit isn't registered in ZContentKitManager::m_GlobalOutfitKits
        if (object.HasMember("outfitVariations"))
        {
            for (const auto& variation : object["outfitVariations"].GetArray())
            {
                const std::string resourceID = variation.GetString();
                const ZRuntimeResourceID runtimeResourceID = SDK().GetRuntimeResourceID(resourceID);

                outfit.m_OutfitVariations.push_back({ runtimeResourceID, resourceID });
            }

            continue;
        }
    }

    for (auto& outfit : m_Outfits)
    {
        auto globalOutfitKitIt = Globals::ContentKitManager->m_GlobalOutfitKits.Find(outfit.m_TokenID);

        if (globalOutfitKitIt == Globals::ContentKitManager->m_GlobalOutfitKits.End())
        {
            continue;
        }

        for (uint32_t i = 0; i < 12; ++i)
        {
            const ZRuntimeResourceID variationResource = globalOutfitKitIt->m_value.m_pInterfaceRef->GetVariationResource(i + 1);

            if (variationResource.GetID() != -1)
            {
                outfit.m_OutfitVariations.push_back({ variationResource, SDK().GetResourceID(variationResource) });
            }
        }
    }
}

void Player::LoadFirearms()
{
    const std::string_view firearmsJson = SDK().GetTextResource(IDR_FIREARMS);

    if (firearmsJson.empty())
    {
        Logger::Error("Failed to load firearms resource.");
        return;
    }

    rapidjson::Document document;
    document.Parse(firearmsJson.data(), firearmsJson.size());

    if (document.HasParseError())
    {
        Logger::Error("Failed to parse firearms resource.");
        return;
    }

    const auto& firearms = document["firearms"].GetArray();

    m_Firearms.reserve(firearms.Size());

    for (const auto& object : firearms)
    {
        Firearm& firearm = m_Firearms.emplace_back();

        firearm.m_Title = object["title"].GetString();
        firearm.m_RuntimeResourceID = object["runtimeResourceID"].GetUint64();
    }
}

void Player::LoadItems()
{
    const std::string_view itemsJson = SDK().GetTextResource(IDR_ITEMS);

    if (itemsJson.empty())
    {
        Logger::Error("Failed to load items resource.");
        return;
    }

    rapidjson::Document document;
    document.Parse(itemsJson.data(), itemsJson.size());

    if (document.HasParseError())
    {
        Logger::Error("Failed to parse items resource.");
        return;
    }

    const auto& items = document["items"].GetArray();

    m_Items.reserve(items.Size());

    for (const auto& object : items)
    {
        Item& item = m_Items.emplace_back();

        item.m_Title = object["title"].GetString();
        item.m_RuntimeResourceID = object["runtimeResourceID"].GetUint64();
    }
}

void Player::LoadActorTypesAndResourceIDs()
{
    const std::string_view actors = SDK().GetTextResource(IDR_ACTORS);

    if (actors.empty())
    {
        Logger::Error("Failed to load actors resource.");
        return;
    }

    size_t start = 0;

    while (start < actors.size())
    {
        const size_t end = actors.find('\n', start);
        std::string_view line = actors.substr(start, end - start);

        if (!line.empty() && line.back() == '\r')
        {
            line.remove_suffix(1);
        }

        const size_t separator = line.find('#');

        if (separator != std::string_view::npos)
        {
            const std::string_view actorType = line.substr(0, separator);
            const std::string_view resourceID = line.substr(separator + 1);

            m_ActorTypeToResourceID.emplace(std::string(actorType), std::string(resourceID));
        }

        if (end == std::string_view::npos)
        {
            break;
        }

        start = end + 1;
    }
}

void Player::EquipOutfit(ZActor* p_Actor)
{
    ZHitman5* hitman = Globals::LevelManager->m_rHitman.m_pInterfaceRef;

    if (p_Actor->IsWearingOutfit())
    {
        hitman->EquipOutfitResource(p_Actor->m_pCharacterTemplate.m_pInterfaceRef->m_HMAsResID, p_Actor->GetOutfit(), false, false);
    }
}

void Player::EquipOutfit(const STokenID& p_TokenID, const ZRuntimeResourceID& p_OutfitVariation)
{
    ZHitman5* hitman = Globals::LevelManager->m_rHitman.m_pInterfaceRef;
    TEntityRef<ZHM5Outfit> outfitEntity = Globals::LevelManager->m_pOutfitManager.m_pInterfaceRef->GetOutfit(p_TokenID);

    hitman->EquipOutfitResource(p_OutfitVariation, outfitEntity.m_entityRef, false, false);
}

void Player::EquipModel(ZActor* p_Actor)
{
    ZHitman5* hitman = Globals::LevelManager->m_rHitman.m_pInterfaceRef;

    // If hitman has suit equip some other outfit so that suit doesn't stay after changing model
    if (p_Actor->IsWearingOutfit())
    {
        hitman->EquipOutfitResource(p_Actor->m_pCharacterTemplate.m_pInterfaceRef->m_HMAsResID, p_Actor->GetOutfit(), false, false);
    }
    else
    {
        const Outfit& outfit = m_Outfits[1];
        const int outfitVariationIndex = 0;
        const ZRuntimeResourceID outfitVariation = outfit.m_OutfitVariations[0].first;

        ZHitman5* hitman = Globals::LevelManager->m_rHitman.m_pInterfaceRef;
        TEntityRef<ZHM5Outfit> outfitEntity = Globals::LevelManager->m_pOutfitManager.m_pInterfaceRef->GetOutfit(outfit.m_TokenID);

        hitman->EquipOutfitResource(outfitVariation, outfitEntity.m_entityRef, false, false);
    }

    ZResourcePtr primResourcePtr = p_Actor->GetGeomEntityPtr()->GetResourcePtr();

    hitman->GetGeomEntityPtr()->ChangePrimitiveResourcePtr(primResourcePtr);
}

void Player::EquipModel(const std::string& p_ResourceID)
{
    ZDynamicResourceLibrary* dynamicResourceLibrary;
    bool isDynamicResourceLibraryInstalled =
        SDK().CreateAndInstallDynamicResourceLibrary(p_ResourceID, dynamicResourceLibrary, m_SourceResourceRuntimeResourceID);

    if (isDynamicResourceLibraryInstalled)
    {
        ZEntityRef entityRef = dynamicResourceLibrary->GetEntity(0);
        ZGeomEntity* geomEntity = entityRef.QueryInterfacePtr<ZGeomEntity>();
        ZHitman5* hitman = Globals::LevelManager->m_rHitman.m_pInterfaceRef;

        if (hitman)
        {
            const Outfit& outfit = m_Outfits[1];
            const int outfitVariationIndex = 0;
            const ZRuntimeResourceID outfitVariation = outfit.m_OutfitVariations[0].first;

            ZHitman5* hitman = Globals::LevelManager->m_rHitman.m_pInterfaceRef;
            TEntityRef<ZHM5Outfit> outfitEntity = Globals::LevelManager->m_pOutfitManager.m_pInterfaceRef->GetOutfit(outfit.m_TokenID);

            hitman->EquipOutfitResource(outfitVariation, outfitEntity.m_entityRef, false, false);

            hitman->GetGeomEntityPtr()->ChangePrimitiveResourcePtr(geomEntity->GetResourcePtr());
        }

        m_DynamicResourceLibraries.push_back(dynamicResourceLibrary);
    }
}

void Player::SpawnFirearm()
{
    ZDynamicResourceLibrary* dynamicResourceLibrary;
    bool isDynamicResourceLibraryInstalled =
        SDK().InstallDynamicResourceLibrary(m_SelectedFirearm, dynamicResourceLibrary, m_SourceResourceRuntimeResourceID, m_FirearmSpawnCount);

    if (isDynamicResourceLibraryInstalled)
    {
        for (unsigned int i = 0; i < dynamicResourceLibrary->m_Entities.Size(); ++i)
        {
            ZEntityRef entityRef = dynamicResourceLibrary->GetEntity(i);
            ZHitman5* hitman = Globals::LevelManager->m_rHitman.m_pInterfaceRef;

            if (hitman && !m_AddFirearmToWorld)
            {
                hitman->m_pBaseInventory->AddItemToInventory(entityRef, false, false);
            }
        }

        m_DynamicResourceLibraries.push_back(dynamicResourceLibrary);
    }
}

void Player::SpawnItem()
{
    ZDynamicResourceLibrary* dynamicResourceLibrary;
    bool isDynamicResourceLibraryInstalled = false;

    if (m_SelectedItem.IsLibraryResource())
    {
        const std::string resourceID = SDK().GetResourceID(m_SelectedItem);

        isDynamicResourceLibraryInstalled =
            SDK().CreateAndInstallDynamicResourceLibrary(resourceID, dynamicResourceLibrary, m_SourceResourceRuntimeResourceID, m_ItemSpawnCount);
    }
    else
    {
        isDynamicResourceLibraryInstalled =
            SDK().InstallDynamicResourceLibrary(m_SelectedItem, dynamicResourceLibrary, m_SourceResourceRuntimeResourceID, m_ItemSpawnCount);
    }

    if (isDynamicResourceLibraryInstalled)
    {
        for (unsigned int i = 0; i < dynamicResourceLibrary->m_Entities.Size(); ++i)
        {
            ZEntityRef entityRef = dynamicResourceLibrary->GetEntity(i);
            IHM5Item* item = entityRef.QueryInterfacePtr<IHM5Item>();
            ZHitman5* hitman = Globals::LevelManager->m_rHitman.m_pInterfaceRef;

            if (hitman && !m_AddItemToWorld)
            {
                hitman->m_pBaseInventory->AddItemToInventory(entityRef, false, false);
            }
        }

        m_DynamicResourceLibraries.push_back(dynamicResourceLibrary);
    }
}

void Player::SetInfiniteAmmo()
{
    ZHitman5* hitman = Globals::Globals::LevelManager->m_rHitman.m_pInterfaceRef;

    if (hitman)
    {
        TFixedArray<uint32_t, 8> ammoInPocket;

        for (uint32_t i = 0; i < 8; ++i)
        {
            ammoInPocket[i] = 999;
        }

        hitman->m_pBaseInventory->SetAmmoInPocket(ammoInPocket);
    }
}

ZActor* Player::FindNearestActor()
{
    const ZSpatialEntity* hitmanSpatialEntity = Globals::LevelManager->m_rHitman.m_pInterfaceRef->GetSpatialEntityPtr();

    for (size_t i = 0; i < Globals::ActorManager->m_aliveActors.Size(); ++i)
    {
        ZActor* actor = Globals::ActorManager->m_aliveActors[i].m_pInterfaceRef;
        const ZSpatialEntity* actorSpatialEntity = actor->GetSpatialEntityPtr();

        const SVector3 offset = hitmanSpatialEntity->GetObjectToWorldMatrix().Trans - actorSpatialEntity->GetObjectToWorldMatrix().Trans;
        const float distance = sqrt(offset.x * offset.x + offset.y * offset.y + offset.z * offset.z);

        if (distance <= 3.f)
        {
            return actor;
        }
    }

    return nullptr;
}

DEFINE_THISCALL_MOD_DETOUR(Player, void, ZEntitySceneContext_ClearScene, ZEntitySceneContext* p_EntitySceneContext, bool p_FullyUnloadScene)
{
    m_ActorName.clear();

    m_SpawnedFirearmCount = 0;
    m_SpawnedItemCount = 0;

    for (auto dynamicResourceLibrary : m_DynamicResourceLibraries)
    {
        dynamicResourceLibrary->~ZDynamicResourceLibrary();
        (*Globals::MemoryManager)->m_pNormalAllocator->Free(dynamicResourceLibrary);
    }

    m_DynamicResourceLibraries.clear();

    return { HookAction::Continue() };
}

DEFINE_THISCALL_MOD_DETOUR(
    Player, ZEntityType**, ZEntityManager_ConstructUninitializedEntity, ZEntityManager* p_EntityManager, const ZString& p_DebugName,
    IEntityFactory* p_EntityFactory, uint8_t* p_MemBlock
)
{
    ZEntityType** entityType = p_Hook->CallOriginal(p_EntityManager, p_DebugName, p_EntityFactory, p_MemBlock);

    if ((m_AddFirearmToWorld || m_AddItemToWorld) && Globals::ZTemplateEntityFactoryVFTbl == *reinterpret_cast<void**>(p_EntityFactory))
    {
        ZTemplateEntityFactory* templateEntityFactory = static_cast<ZTemplateEntityFactory*>(p_EntityFactory);
        const ZRuntimeResourceID templateRuntimeResourceID = templateEntityFactory->m_ridResource;

        if (templateRuntimeResourceID == m_SourceResourceRuntimeResourceID)
        {
            ZHitman5* hitman = Globals::LevelManager->m_rHitman.m_pInterfaceRef;

            if (hitman)
            {
                ZEntityRef entityRef = ZEntityRef(entityType);
                ZSpatialEntity* hitmanSpatialEntity = hitman->GetSpatialEntityPtr();
                SMatrix43 transform;

                transform.Trans = hitmanSpatialEntity->GetWorldPosition() - float4(0.f, 0.5f, 0.f, 0.f);

                entityRef.SetProperty("m_mTransform", transform);
            }
        }

        if (m_SpawnFirearm)
        {
            if (m_SpawnedFirearmCount == m_FirearmSpawnCount)
            {
                m_SpawnedFirearmCount = 1;

                if (m_SpawnFirearm)
                {
                    m_SpawnFirearm = false;
                }
            }
            else
            {
                ++m_SpawnedFirearmCount;
            }
        }
        else if (m_SpawnItem)
        {
            if (m_SpawnedItemCount == m_ItemSpawnCount)
            {
                m_SpawnedItemCount = 1;

                if (m_SpawnItem)
                {
                    m_SpawnItem = false;
                }
            }
            else
            {
                ++m_SpawnedItemCount;
            }
        }
    }

    return { HookAction::Return(), entityType };
}

DEFINE_THISCALL_MOD_DETOUR(Player, void, ZHM5ReloadController_EndReloadWeapon, ZHM5ReloadController* p_HM5ReloadController)
{
    p_Hook->CallOriginal(p_HM5ReloadController);

    if (m_IsInfiniteAmmoEnabled)
    {
        SetInfiniteAmmo();
    }

    return { HookAction::Return() };
}

DEFINE_HMASDK_MOD(Player);
